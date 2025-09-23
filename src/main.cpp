#include <WiFi.h>          // pour ESP32 (ou <ESP8266WiFi.h> pour ESP8266)
#include <PubSubClient.h>

//______________Partie WiFi________________//
const char* ssid = "BTS_CIEL_IR_ESP32";
const char* password = "btscieliresp32";
//--------------Fin partie WiFi----------------//

//_______________Partie MQTT________________//
const char* mqtt_server = "192.168.3.73";  

const char* mqtt_user = "monutilisateur";      // Remplace par ton nom d'utilisateur MQTT
const char* mqtt_password = "pwd_MQTT"; 

WiFiClient espClient;
PubSubClient client(espClient);

long lastReconnectAttempt = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

//--------------Fin partie MQTT----------------//


//_______________Test MQTT________________//
boolean reconnect() {
  if (client.connect("espClient", mqtt_user, mqtt_password)) {
    // Une fois connecté, on publie un message
    client.publish("esp32/bme280/temperature", "32");
    client.publish("esp32/bme280/humidity", "60");
    client.publish("esp32/bme280/pression", "1050");
    // ... et on s'abonne au topic
    client.subscribe("inTopic");
  }
  return client.connected();
}

//--------------Fin test MQTT----------------//



void setup() {
  //______________Partie WiFi________________//
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connexion au WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connecté au WiFi");

  //--------------Fin partie WiFi----------------//


  //______________Partie MQTT________________//
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  lastReconnectAttempt = 0;

  //--------------Fin partie MQTT----------------//
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
