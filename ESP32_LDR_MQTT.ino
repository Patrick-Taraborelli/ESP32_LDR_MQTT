#include <WiFi.h>
#include <PubSubClient.h>

#define LDRPIN 32

// Definições do WiFi e MQTT
const char* ssid = "####";
const char* password = "####";
const char* mqttServer = "192.168.0.103";
const int mqttPort = 1884;
const char* mqttUser = "####"; 
const char* mqttPassword = "####";
const char* mqttTopic = "esp32/ldr/light";


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  analogSetAttenuation(ADC_11db);

  setupWiFi();

  client.setServer(mqttServer, mqttPort);
}

void setupWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("Conectado ao WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {

  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Conectado ao MQTT");
    } else {
      Serial.print("Falha na conexão, rc=");
      Serial.print(client.state());
      Serial.println(" Tentar novamente em 5 segundos");
      delay(5000);
    }
  }
}

void reconnectWiFi() {

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Reconectando ao WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    
    for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) { 
      delay(500);
      Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Reconectado ao WiFi");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("Falha ao reconectar ao WiFi, tentando novamente...");
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // reads the input on analog pin (value between 0 and 4095)
  int analogValue = analogRead(LDRPIN);

  Serial.print("\n Analog Value = ");
  Serial.print(analogValue);   // the raw analog reading

  float luxValue = analogValue / 8.75; // Conversion to lux

  String msg = String(luxValue);
  client.publish(mqttTopic, msg.c_str());


  Serial.printf("\n Publicando no tópico %s: %.2f lux\n", mqttTopic, luxValue);

  delay(1000);
}
