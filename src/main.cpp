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
int temperature = 0;

bool reconnect() {
  if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
    // Subscribe to topics if needed
    // client.subscribe("your/topic");
    return true;
  }
  return false;
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

  while (temperature <= 50) {
    temperature++;
    Serial.print("Température: ");
    Serial.println(temperature);
    // Publier la température sur le topic "capteur/temperature"
    client.publish("esp32/bme280/temperature", String(temperature).c_str());
    delay(1000); // Attendre 2 secondes avant la prochaine lecture
  }
  temperature = 0; // Réinitialiser la température pour la prochaine boucle
}
