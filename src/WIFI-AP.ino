#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Wire.h>

const int ledOutputPin = 23;     // Pino do LED que acende durante a rotina
const int readyLedPin = 18;      // Pino do LED que indica que o sistema está pronto
const int runningLedPin = 19;  // Pino da LED que indica que a rotina está em execução
const int esp32_led = 2; // led ESP32
const int runningLed2Pin = 5;  // Pino do segundo LED de execução (novo)

#define BUTTON_PIN 34  // Pino para o botão de reset

WebServer server(80);  // Servidor web na porta 80
Preferences preferences;  // Para armazenar configurações de Wi-Fi

// Definições do ponto de acesso
const char *ssidAP = "ESP32_Config";  // Nome do ponto de acesso
const char *passAP = "123456789";     // Senha do ponto de acesso

// Variáveis para armazenar SSID e senha do Wi-Fi
String ssid = "";
String password = "";

// Variáveis para controle do botão
bool lastButtonState = HIGH;  // Último estado do botão (estado inicial é HIGH)
bool buttonPressed = false;   // Flag para verificar se o botão foi pressionado
unsigned long buttonPressTime = 0;  // Armazenar o tempo de pressionamento do botão

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação serial

  pinMode(ledOutputPin, OUTPUT);
  pinMode(esp32_led, OUTPUT);
  pinMode(readyLedPin, OUTPUT); // Configura o pino do LED de pronto
  pinMode(runningLedPin, OUTPUT);  // Configura o pino da LED como saída
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);  // Configura o pino como entrada com pull-down (quando pressionado, pino vai para LOW)

  loadWiFiConfig();  // Carrega as configurações de Wi-Fi salvas

  // Verifica se o ESP32 está conectado ao Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado ao Wi-Fi");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Sem conexão Wi-Fi, aguardando configurações...");
  }

  // Se não estiver conectado, cria o ponto de acesso
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Configuração do ponto de acesso iniciada.");
    Serial.print("IP do AP: ");
    Serial.println(WiFi.softAPIP());
  }

  // Configura as rotas do servidor web
  server.on("/", HTTP_GET, handleRoot);  // Página principal de configuração
  server.on("/config", HTTP_POST, handleConfig);  // Processa a configuração

  server.begin();  // Inicia o servidor web
}

void loop() {
  server.handleClient();  // Processa requisições de clientes (servidor web)

  // Se as configurações de Wi-Fi foram encontradas, tenta se conectar
  if (WiFi.status() != WL_CONNECTED && ssid != "" && password != "") {
    Serial.println("Wi-Fi desconectado. Tentando reconectar...");
    WiFi.begin(ssid.c_str(), password.c_str());  // Tenta reconectar ao Wi-Fi
    unsigned long startAttemptTime = millis();

    // Aguarda até 10 segundos para tentar reconectar
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);  // Aguarda 500ms antes de tentar novamente
      Serial.print(".");  // Exibe um ponto enquanto tenta
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Reconectado ao Wi-Fi!");
    } else {
      Serial.println("Falha ao reconectar ao Wi-Fi.");
    }
  }

  // Lê o estado do botão
  bool buttonState = digitalRead(BUTTON_PIN);

  // Verifica se houve uma mudança no estado do botão
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {  // O botão foi pressionado
      // Registra o tempo de pressionamento do botão
      buttonPressTime = millis();
      buttonPressed = true;
    }
    delay(50);  // Adiciona um pequeno delay para debounce
  }

  // Se o botão foi pressionado por mais de 3 segundos, limpa configurações e reinicia
  if (buttonPressed && millis() - buttonPressTime > 3000) {
    digitalWrite(readyLedPin, HIGH);
    digitalWrite(runningLedPin, HIGH);
    digitalWrite(esp32_led, HIGH);
    Serial.println("Botão pressionado por 3 segundos! Limpando preferências...");
    clearWiFiConfig();  // Limpa as configurações salvas
    delay(2000);  // Espera um pouco antes de reiniciar
    digitalWrite(readyLedPin, LOW);
    digitalWrite(runningLedPin, LOW);
    digitalWrite(esp32_led, LOW);
    ESP.restart();  // Reinicia o ESP32
  }

  lastButtonState = buttonState;  // Atualiza o último estado do botão
}

// Função para limpar as configurações salvas de Wi-Fi
void clearWiFiConfig() {
  preferences.begin("wifi", false);  // Inicia o namespace para escrita
  preferences.clear();  // Limpa todas as configurações salvas
  preferences.end();  // Finaliza a operação
}

// Função para carregar as configurações de Wi-Fi salvas
void loadWiFiConfig() {
  preferences.begin("wifi", true);  // Inicia o namespace para leitura
  ssid = preferences.getString("ssid", "");  // Obtém o SSID
  password = preferences.getString("password", "");  // Obtém a senha
  preferences.end();  // Finaliza a operação

  if (ssid != "" && password != "") {
    // Se existirem configurações de Wi-Fi salvas, tenta conectar
    WiFi.begin(ssid.c_str(), password.c_str());  // Tenta conectar ao Wi-Fi
  } else {
    // Se não houver configurações salvas, cria o ponto de acesso
    Serial.println("Sem configurações salvas, configurando ponto de acesso...");
    WiFi.softAP(ssidAP, passAP);  // Configura o ponto de acesso
  }
}

// Função que exibe a página de configuração
// Função que exibe a página de configuração
void handleRoot() {
  String html = "<html><head><meta charset=\"UTF-8\"><h1>Configuração Wi-Fi</h1></head><body>";
  html += "<form action=\"/config\" method=\"POST\">";
  html += "SSID: <input type=\"text\" name=\"ssid\"><br>";
  html += "Senha: <input type=\"password\" name=\"password\"><br>";
  html += "<input type=\"submit\" value=\"Salvar\">";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}


// Função que salva as configurações de Wi-Fi
void handleConfig() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String newSSID = server.arg("ssid");
    String newPassword = server.arg("password");

    preferences.begin("wifi", false);  // Inicia o namespace para escrita
    preferences.putString("ssid", newSSID);  // Salva o SSID
    preferences.putString("password", newPassword);  // Salva a senha
    preferences.end();  // Finaliza a operação

    WiFi.begin(newSSID.c_str(), newPassword.c_str());  // Tenta conectar ao novo Wi-Fi
    server.send(200, "text/html", "<html><head><meta charset=\"UTF-8\"><body><h1>Configurações Salvas! Reiniciando...</h1></head></body></html>");
    delay(1000);  // Aguarda um pouco antes de reiniciar
    ESP.restart();  // Reinicia o ESP32
  } else {
    server.send(400, "text/html", "<html><body><h1>Erro! SSID e Senha são obrigatórios.</h1></body></html>");
  }
}
