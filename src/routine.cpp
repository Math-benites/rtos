#include <Arduino.h>
#include "routine.h"

const int ledOutputPin = 23;
const int readyLedPin = 18;
const int runningLedPin = 19;
const int runningLed2Pin = 5;

volatile bool startRoutine = false;
volatile bool isRoutineRunning = false;

extern int credit; // Adicionando a variável 'credit' que vem do 'buttons.cpp'

void initRoutine() {
    pinMode(ledOutputPin, OUTPUT);
    pinMode(readyLedPin, OUTPUT);
    pinMode(runningLedPin, OUTPUT);
    pinMode(runningLed2Pin, OUTPUT);
}

void routineTask(void *pvParameters) {
    for (;;) {
        if (startRoutine) {
            startRoutine = false;
            isRoutineRunning = true;

            digitalWrite(runningLedPin, HIGH);
            digitalWrite(readyLedPin, LOW);  // Desliga o LED quando a rotina começar
            digitalWrite(ledOutputPin, HIGH);

            vTaskDelay(6000 / portTICK_PERIOD_MS);

            digitalWrite(runningLed2Pin, HIGH);

            vTaskDelay(4000 / portTICK_PERIOD_MS);

            digitalWrite(runningLedPin, LOW);
            digitalWrite(runningLed2Pin, LOW);
            digitalWrite(ledOutputPin, LOW);

            isRoutineRunning = false;
        }

        // Controle do LED "readyLedPin" com base no crédito
        if (credit > 0) {
            digitalWrite(readyLedPin, HIGH);  // Acende o LED quando há crédito
        } else {
            digitalWrite(readyLedPin, LOW);   // Apaga o LED quando não há crédito
        }

        vTaskDelay(100 / portTICK_PERIOD_MS); // Atualiza a cada 100ms
    }
}
