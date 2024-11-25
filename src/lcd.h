#ifndef LCD_H
#define LCD_H

void initLCD(); // Inicializa o LCD
void lcdTask(void *pvParameters); // Tarefa para gerenciar o LCD
void updateCreditDisplay(int credit); // Atualiza o display com os créditos

#endif
