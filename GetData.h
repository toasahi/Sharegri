/*
 * 土の水分量を測定する
 * int soilPin 制御ピン
 * return moist 水分量
 */

int readSoil(int soilPin) {
  delay(10);
  int val = analogRead(soilPin);
  int moist = map(val, 0, 4095, 0, 99);
  moist = 100 - moist;
  return moist;
}
