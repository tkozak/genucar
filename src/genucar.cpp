// include the library code:
#include <Arduino.h>
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 2, en = 3;
const int d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
char line1[17]="                ";
char line2[17]="                ";

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  Serial.begin(9600);

}

void loop() {
  while (Serial.available() > 0) {
    memcpy(line1, line2, sizeof(line2));
    for (int i=0; i<17; i++) {
      line2[i]=' ';
    }
    Serial.readBytesUntil('\n',line2,16);
    lcd.setCursor(0,0);
    lcd.write(line1);
    lcd.setCursor(0,1);
    lcd.write(line2);
  }
}
