#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Defina o nome da rede Wi-Fi (SSID) e a senha
const char* ssid = "Benites";
const char* password = "pull2221";

// Defina o endereço IP do broker MQTT e a porta
const char* mqtt_server = "192.168.1.3";
const int mqtt_port = 1883;

// Variáveis globais para cliente Wi-Fi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void connectToWiFi() {
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);

  // Aguarda a conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado ao Wi-Fi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void connectToMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT... ");
    if (client.connect("ESP32Client")) { // Nome do cliente MQTT
      Serial.println("Conectado!");
    } else {
      Serial.print("Falhou. Código de erro: ");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();

  // Configura o broker MQTT
  client.setServer(mqtt_server, mqtt_port);
  connectToMQTT();
}

void loop() {
  // Mantém a conexão MQTT ativa
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  // Publica uma mensagem no tópico "test/topic"
  client.publish("test/topic", "Hello from ESP32!");
  delay(5000); // Aguarda 5 segundos antes de publicar novamente
}
