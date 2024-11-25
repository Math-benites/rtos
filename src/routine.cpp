#include <Arduino.h>
#include "routine.h"

const int ledOutputPin = 23;
const int readyLedPin = 18;
const int runningLedPin = 19;
const int runningLed2Pin = 5;

volatile bool startRoutine = false;
volatile bool isRoutineRunning = false;

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
            digitalWrite(readyLedPin, LOW);
            digitalWrite(ledOutputPin, HIGH);

            vTaskDelay(6000 / portTICK_PERIOD_MS);

            digitalWrite(runningLed2Pin, HIGH);

            vTaskDelay(4000 / portTICK_PERIOD_MS);

            digitalWrite(runningLedPin, LOW);
            digitalWrite(runningLed2Pin, LOW);
            digitalWrite(ledOutputPin, LOW);

            isRoutineRunning = false;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
