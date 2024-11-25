#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "lcd.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
extern int credit;

void initLCD() {
    lcd.init();
    lcd.backlight();
    lcd.print("Creditos: ");
    lcd.setCursor(0, 1);
    lcd.print(credit);
}

void lcdTask(void *pvParameters) {
    for (;;) {
        lcd.setCursor(9, 0);
        lcd.print(credit);
        lcd.print("   ");

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void updateCreditDisplay(int credit) {
    lcd.setCursor(9, 0);
    lcd.print(credit);
    lcd.print("   ");
}
