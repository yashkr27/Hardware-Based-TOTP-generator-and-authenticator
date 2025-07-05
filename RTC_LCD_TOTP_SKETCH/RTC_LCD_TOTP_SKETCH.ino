#include <Wire.h>
#include <RTClib.h>
#include <TOTP.h>
#include <LiquidCrystal.h>
#include <Crypto.h>

// === LCD Pin Setup ===
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// === RTC & TOTP Setup ===
RTC_DS3231 rtc;
uint8_t hmacKey[] = { '1','2','3','4','5','6','7','8','9','0' };  // Replace with your actual secret
TOTP totp(hmacKey, sizeof(hmacKey));

void setup() {
  lcd.begin(16, 2);
  Wire.begin();

  Serial.begin(9600);
  while (!Serial);  // Wait for Serial monitor (Leonardo requirement)

  if (!rtc.begin()) {
    lcd.print("RTC not found");
    while (1);  // Halt if RTC not detected
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TOTP Generator");
  delay(2000);
  lcd.clear();
}

void loop() {
  // 🟡 1. Check if Electron sent a service name
  if (Serial.available()) {
    String service = Serial.readStringUntil('\n');
    Serial.print("Service selected: ");
    Serial.println(service);
    // You could switch secrets here based on service
  }

  // 🕒 2. Get current time
  DateTime now = rtc.now();
  uint32_t timestamp = now.unixtime();

  // 🔐 3. Generate OTP
  char* code = totp.getCode(timestamp);

  // 📺 4. Show on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("OTP: ");
  lcd.print(code);

  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());

  // 🛜 5. Send OTP to Electron GUI
  Serial.println(code);

  delay(30000);  // Refresh every 30s
}
