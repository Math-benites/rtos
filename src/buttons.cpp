#include <Arduino.h>
#include "buttons.h"
#include "routine.h" // Para interagir com o controle de rotina
#include "lcd.h"     // Para atualizar o LCD

const int creditButtonPin = 15;
const int startButtonPin = 4;

extern int credit;
extern volatile bool startRoutine;
extern volatile bool isRoutineRunning;

void initButtons() {
    pinMode(creditButtonPin, INPUT_PULLUP);
    pinMode(startButtonPin, INPUT_PULLUP);
}

void buttonTask(void *pvParameters) {
    const int debounceDelay = 1000;
    unsigned long lastButtonPress = 0;
    unsigned long lastStartPress = 0;

    for (;;) {
        if (digitalRead(creditButtonPin) == LOW) {
            if (millis() - lastButtonPress > debounceDelay) {
                credit++;
                updateCreditDisplay(credit);
                Serial.println("Créditos: " + String(credit));
                lastButtonPress = millis();
            }
        }

        if (digitalRead(startButtonPin) == LOW) {
            if (millis() - lastStartPress > debounceDelay && !isRoutineRunning) {
                if (credit > 0) {
                    credit--;
                    startRoutine = true;
                    updateCreditDisplay(credit);
                }
                lastStartPress = millis();
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
