#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Protótipos das funções
void update_menu_lcd();
void checkButton();
void checkStartButton();
void displayCredits();
void readDHTSensor(); 
void rotina();
 
// Endereço I2C do LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Definindo o tipo de sensor DHT e o pino em que ele está conectado
#define DHTPIN 17          // Pino do DHT22 (ajuste conforme necessário)
#define DHTTYPE DHT22      // Definindo o tipo de sensor

const int creditButtonPin = 15; // Pino do botão para adicionar crédito
const int startButtonPin = 4;   // Pino do botão de start
const int ledOutputPin = 23;     // Pino do LED que acende durante a rotina
const int readyLedPin = 18;      // Pino do LED que indica que o sistema está pronto
const int runningLedPin = 19;  // Pino da LED que indica que a rotina está em execução
const int runningLed2Pin = 5;  // Pino do segundo LED de execução (novo)
const int esp32_led = 2; // led ESP32

int credit = 0;                 // Variável para armazenar créditos
DHT dht(DHTPIN, DHTTYPE);

// Variáveis para controle de tempo e anti-bounce
unsigned long lastButtonPress = 0;
unsigned long lastStartPress = 0;
const int debounceDelay = 1000;  // Delay de anti-bounce em milissegundos
const int ledDuration = 10000;   // Duração do LED ligado em milissegundos
unsigned long lastDHTRead = 0; // Tempo da última leitura do DHT
const unsigned long dhtReadInterval = 5000; // Intervalo para ler DHT em milissegundos

void setup() {
    Serial.begin(115200);
    pinMode(creditButtonPin, INPUT_PULLUP);
    pinMode(startButtonPin, INPUT_PULLUP);
    pinMode(ledOutputPin, OUTPUT);
    pinMode(readyLedPin, OUTPUT); // Configura o pino do LED de pronto
    pinMode(runningLedPin, OUTPUT);  // Configura o pino da LED como saída
    pinMode(runningLed2Pin, OUTPUT);  // Configura o pino do segundo LED como saída

    lcd.init();
    lcd.backlight();
    lcd.print("Creditos");
    lcd.setCursor(0, 1);
    lcd.print(credit);
    dht.begin();  // Inicializa o sensor DHT
}

void loop() {
    checkButton();      // Verifica se o botão de crédito foi pressionado
    checkStartButton(); // Verifica se o botão de start foi pressionado
    update_menu_lcd();  // Atualiza o menu do LCD
}

// Definições de funções (mantidas conforme o código original)
void update_menu_lcd() {
    if (credit > 0) {
        digitalWrite(readyLedPin, HIGH); // Liga o LED se houver crédito
    } else {
        digitalWrite(readyLedPin, LOW);  // Desliga o LED se não houver crédito
    }

    displayCredits();
    if (millis() - lastDHTRead >= dhtReadInterval) {
        readDHTSensor(); // Chama a função de leitura do DHT
        lastDHTRead = millis(); // Atualiza o tempo da última leitura
    }
}

void checkButton() {
    if (digitalRead(creditButtonPin) == LOW) {  
        if (millis() - lastButtonPress >= debounceDelay) {
            credit += 1;
            Serial.print("Creditos: ");
            Serial.println(credit);
            displayCredits();
            lastButtonPress = millis();
        }
    }
}

void checkStartButton() {
    if (digitalRead(startButtonPin) == LOW && credit > 0) {  
        if (millis() - lastStartPress >= debounceDelay) {
            credit -= 1;
            Serial.print("Créditos: ");
            Serial.println(credit);
            displayCredits();
            rotina();
            lastStartPress = millis();
        }
    }
}

void rotina() {
    digitalWrite(runningLedPin, HIGH);
    digitalWrite(readyLedPin, LOW);

    digitalWrite(ledOutputPin, HIGH);
    delay(6000);
    digitalWrite(runningLed2Pin, HIGH);
    delay(ledDuration - 6000);

    digitalWrite(ledOutputPin, LOW);
    digitalWrite(runningLedPin, LOW);
    digitalWrite(runningLed2Pin, LOW);

    if (credit > 0) {
        digitalWrite(readyLedPin, HIGH);
    }
}

void displayCredits() {
    lcd.setCursor(9, 0);
    lcd.print(credit);
    lcd.print("   ");
}

void readDHTSensor() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
        Serial.println("Falha ao ler do DHT sensor!");
        return;
    }

    Serial.print("Umidade: ");
    Serial.print(h);
    Serial.print("%  Temperatura: ");
    Serial.print(t);
    Serial.println("°C");

    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.print("C  ");
}
