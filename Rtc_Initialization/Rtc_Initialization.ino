#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(2000);

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1);
  }

  // Set RTC time to the time when this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  Serial.println("RTC time initialized to compile time.");
}

void loop() {
  DateTime now = rtc.now();
  Serial.print("RTC Time: ");
  Serial.print(now.year()); Serial.print('/');
  Serial.print(now.month()); Serial.print('/');
  Serial.print(now.day()); Serial.print(" ");
  Serial.print(now.hour()); Serial.print(':');
  Serial.print(now.minute()); Serial.print(':');
  Serial.println(now.second());

  delay(1000);
}
