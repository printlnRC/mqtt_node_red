#include <WiFi.h>          // pour ESP32 (ou <ESP8266WiFi.h> pour ESP8266)
#include <PubSubClient.h>

// Remplace par ton SSID et mot de passe WiFi
const char* ssid = "BTS_CIEL_IR_ESP32";
const char* password = "btscieliresp32";

// Adresse IP du serveur MQTT
const char* mqtt_server = "192.168.3.73";  

WiFiClient espClient;
PubSubClient client(espClient);

long lastReconnectAttempt = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

boolean reconnect() {
  if (client.connect("espClient")) {
    // Une fois connecté, on publie un message
    client.publish("esp32/bme280/temperature", "32");
    // ... et on s'abonne au topic
    client.subscribe("inTopic");
  }
  return client.connected();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connexion au WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connecté au WiFi");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  lastReconnectAttempt = 0;
}

void loop() {
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    client.loop();
  }
}
