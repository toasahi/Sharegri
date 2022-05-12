#include "Setting.h"
#include <WiFiClientSecure.h>
#include "esp_deep_sleep.h"

void setUp(const int ledPin){
  pinMode(ledPin, OUTPUT);
}


/* 現在の時刻を取得
 * return currentTime(時)
 */

unsigned long getTime(){
  time_t t;
  struct tm *tm;
  static const char *wd[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};
  t = time(NULL);
  tm = localtime(&t);
  unsigned long currentTime = tm->tm_hour;
  return currentTime;
}

/*
 * ESP32のMacアドレスをIDに変換する
 * return chipId  
 */

uint32_t getChipId(){
  uint32_t chipId = 0;
  for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return chipId;
}

/*
 * ESP32をWiFiに接続する関数
 * const char *ssid  
 * const char *password   
 */

void Wifi_Set(const char *ssid,const char *password){
  //WiFi接続
  Serial.print("Connecting to");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  int wifi_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    //通信状況が悪い時リスタート
    if (++wifi_count > 30) {
      Serial.print("リスタート");
      esp_deep_sleep_enable_timer_wakeup(1 * 1000 * 1000);
      esp_deep_sleep_start();
    }
  }
  Serial.println("Connected");
}
