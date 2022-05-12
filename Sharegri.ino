#include <WiFi.h>
#include "ESP32_BME280_SPI.h"
#include "Wifi_Set.h"
#include "Setting.h"
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#define JST 3600*9 //日本標準時子午線
#define uS_TO_S_FACTOR 1000000ULL //マイクロ->秒に変換係数
#define TIME_TO_SLEEP 5 //ディープスリープ時間()

HTTPClient https;
WiFiClientSecure client;

//WiFi設定
const char ssid[] = "**********";
const char password[] = "**********";;

//Pin配置
const int ledPin = 5;

//Type
typedef enum{
  OPEN,
  CLOSE
}DOOR_STATUS;

void setup() {
  Serial.begin(115200);
  //WiFi接続
  Wifi_Set(ssid, password);
  pinMode(ledPin, OUTPUT);
  //日本時間の設定
  configTime( JST, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
  configTzTime("JST-9", "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
}

void loop() {
  Serial.println(getTime());
}
