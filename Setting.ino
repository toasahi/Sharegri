#include "Setting.h"
void setUp(const int ledPin){
  pinMode(ledPin, OUTPUT);
}

unsigned long getTime(){
  time_t t;
  struct tm *tm;
  static const char *wd[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};
  t = time(NULL);
  tm = localtime(&t);
  unsigned long currentTime = tm->tm_hour;
  return currentTime;
}
