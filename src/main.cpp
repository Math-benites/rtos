#include <Arduino.h>
#include "buttons.h"
#include "lcd.h"
#include "routine.h"

int credit = 0;  // Definindo a variável global 'credit'

// Inicialização no setup
void setup() {
    Serial.begin(115200);

    // Inicializa módulos
    initButtons();
    initLCD();
    initRoutine();

    // Cria as tarefas do RTOS
    xTaskCreate(buttonTask, "Button Task", 2048, NULL, 1, NULL);
    xTaskCreate(lcdTask, "LCD Task", 2048, NULL, 1, NULL);
    xTaskCreate(routineTask, "Routine Task", 2048, NULL, 1, NULL);
}

void loop() {
    // O loop permanece vazio
}
