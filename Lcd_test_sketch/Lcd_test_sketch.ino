#include <LiquidCrystal.h>

// LCD pin connections: rs, en, d4, d5, d6, d7
LiquidCrystal lcd(7, 6, 5, 4, 9, 8);

void setup() {
  lcd.begin(16, 2);              // Initialize 16x2 LCD
  lcd.clear();
  lcd.print("LCD Test OK!");     // First line
  lcd.setCursor(0, 1);           // Move to second line
  lcd.print("Hello, AASTHA!");
}

void loop() {
  // Nothing here; static display test
}
