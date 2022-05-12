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

/*
 * 気温測定
 * return temperature 気温
 */

byte getTemerature(){
  byte temperature = (byte)round(bme280spi.Read_Temperature());
  byte humidity = (byte)round(bme280spi.Read_Humidity());
  uint16_t pressure = (uint16_t)round(bme280spi.Read_Pressure());
  int bmeData[] = {temperature,humidity,pressure};
  return temperature;
}

/*
 * 湿度測定
 * return humidity 湿度
 */
 
byte getHumidity(){
  byte humidity = (byte)round(bme280spi.Read_Humidity());
  return humidity;
}

/*
 * 気圧測定
 * return airPressure 気圧
 */

uint16_t getAirPressure(){
  uint16_t airPressure = (uint16_t)round(bme280spi.Read_Pressure());
  return airPressure;
}

