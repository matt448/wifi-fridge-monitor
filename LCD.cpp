#include "Arduino.h"
#include "pin_definitions.h"
#include <LiquidCrystal.h>

extern float fridgeF;
extern float freezerF;
extern float ambientF;
extern long rssi;

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void lcdSetup(){
  lcd.begin(LCD_W, LCD_H);
}

void lcdBlankLine(int linenum) {
  lcd.setCursor(0, linenum);
  lcd.print("                    ");
}

void lcdBlankScreen() {
  for(int i=0; i < LCD_H; i++){
    lcdBlankLine(i);
  }
}

void lcdDrawStartScreen() {
  lcd.setCursor(0,0);
  lcd.print("WIFI FRIDGE MONITOR");
  lcd.setCursor(0,2);
  lcd.print(" Starting WiFi...");
}

void lcdDrawScreen1() {
  lcdBlankLine(0);
  lcd.setCursor(0,0);
  lcd.print(" FRIDGE: ");
  lcd.print(fridgeF);

  lcdBlankLine(1);
  lcd.setCursor(0,1);
  lcd.print("FREEZER: ");
  lcd.print(freezerF);

  lcdBlankLine(2);
  lcd.setCursor(0,2);
  lcd.print("AMBIENT: ");
  lcd.print(ambientF);

  lcdBlankLine(3);
  lcd.setCursor(0,3);
  lcd.print("   WIFI: ");
  lcd.print(rssi);
}
