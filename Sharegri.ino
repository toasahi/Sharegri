#include <WiFi.h>
#include "ESP32_BME280_SPI.h"
#include "Setting.h"
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#define JST 3600*9 //日本標準時子午線
#define uS_TO_S_FACTOR 1000000ULL //マイクロ->秒に変換係数
#define TIME_TO_SLEEP 5 //ディープスリープ時間()

HTTPClient https;
WiFiClientSecure client;
JSONVar userStatus;
JSONVar keys;

//WiFi設定
const char ssid[] = "";
const char password[] = "";

//Line Notify
const char* lineHost = "notify-api.line.me";
const char* token = "";
const char* message = "Push!";


//Pin配置
const int ledPin = 5;
int soilPin = A0; //土壌センサー制御ピン
int soilPower = 7; //土壌水分用変数
int motPin = 16; //モーター制御ピン

const char host[] = "webページ";
int timeStatus = 0;

//SPI通信用変数設定
const uint8_t SCLK_bme280 = 14; //クロック入力
const uint8_t MOSI_bme280 = 13; //マスター(ESP32):出力,スレーブ(BME280):入力
const uint8_t MISO_bme280 = 12; //マスター(ESP32):入力、スレーブ(BME280):出力
const uint8_t CS_bme280 = 26; //チップ選択

//各種BME280測定モード設定用変数設定
uint8_t t_sb = 5; //測定間でのスタンバイ時間 (5==1000ms)
uint8_t filter = 0; //測定精度(0==off)
uint8_t osrs_t = 4; //温度のオーバーサンプリング値 Temperature x4 ノイズ影響を抑える
uint8_t osrs_p = 4; //気圧のオーバーサンプリング値 Pressure x4
uint8_t osrs_h = 4; //湿度のオーバーサンプリング値 Humidity x4
uint8_t Mode = 3; //計測=ノーマルモード

ESP32_BME280_SPI bme280spi(SCLK_bme280, MOSI_bme280, MISO_bme280, CS_bme280, 10000000);

//Type
typedef enum {
  OPEN,
  CLOSE
} DOOR_STATUS;

void setup() {
  Serial.begin(115200);
  pinMode(motPin, OUTPUT);
  userStatus =  JSON.parse("{\"user1\":\"ok\",\"user2\":\"no\"}");
  keys = userStatus.keys();
  //WiFi接続
  Wifi_Set(ssid, password);
  bme280spi.ESP32_BME280_SPI_Init(t_sb, filter, osrs_t, osrs_p, osrs_h, Mode);
  //日本時間の設定
  configTime( JST, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
  configTzTime("JST-9", "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
  timeStatus = getTime();
}

void sendPostMoisture() {
  String url = "";
  if (client.connect(host, 443)) {
    Serial.println("Connect");
    int bmeData[3];
    getBmeData(bmeData);
    String data;
    data += "moisture=";
    data += readSoil(soilPin);
    data += "&temperature=";
    data += bmeData[0];
    data += "&humidity=";
    data += bmeData[1];
    data += "&air_pressure=";
    data += bmeData[2];
    data += "&chip_id=";
    data += getChipId();
    client.println("POST " + url + "?" + data + " HTTP/1.1");
    client.println("Host: " + (String)host);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("User-Agent: ESP8266/1.0");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println("Connection: close");
    client.println();
    client.println(data);

    client.stop();
    delay(1000);
  } else {
    Serial.println("Client Connect 失敗");
  }
}

void sendLineNotify(){
  client.setInsecure();
  Serial.println("Try");
  
  //LineのAPIサーバにSSL接続（ポート443:https）
  if (!client.connect(lineHost, 443)) {
    Serial.println("Connection failed");
    return;
  }
  
   String query = String("message=") + String(message);
   String request = String("") +
                 "POST /api/notify HTTP/1.1\r\n" +
                 "Host: " + lineHost + "\r\n" +
                 "Authorization: Bearer " + token + "\r\n" +
                 "Content-Length: " + String(query.length()) +  "\r\n" + 
                 "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
                  query + "\r\n";
    client.print(request);

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line == "\r") {
        break;
      }
    }

    String line = client.readStringUntil('\n');
    Serial.println(line);
    delay(500);
}

void loop() {
  
  /*ユーザーの操作
    どの畑に水やりをするのか or 水やりを止めるか
  */
  for (uint8_t i = 0; i < keys.length(); i++) {
    String val = (const char*)userStatus[keys[i]];
    if(val == "ok"){
      Serial.println("開放");
      digitalWrite(motPin, HIGH);
    }else{
      Serial.println("閉鎖");
      digitalWrite(motPin, LOW);
    }
  }
  
  /*データの送信
    畑の状態を送信する
    1時間おき
    畑の一つ一つ
  */
  if(timeStatus != getTime()){
    timeStatus = getTime();
    Serial.println(timeStatus);
    sendLineNotify();
  }
  delay(5000);
}
