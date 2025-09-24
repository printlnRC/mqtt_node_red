#include <WiFi.h>          // pour ESP32 (ou <ESP8266WiFi.h> pour ESP8266)
#include <PubSubClient.h>  // c'est la bibliothèque MQTT

//______________Partie WiFi________________//
const char* ssid = "BTS_CIEL_IR_ESP32"; // nom du réseau WiFi
const char* password = "btscieliresp32"; // mot de passe du réseau WiFi
//--------------Fin partie WiFi----------------//

//_______________Partie MQTT________________//
const char* mqtt_server = "192.168.3.73";  

const char* mqtt_user = "monutilisateur";  // nom d'utilisateur MQTT
const char* mqtt_password = "pwd_MQTT";   // mot de passe MQTT

WiFiClient espClient;  // sert à crée un connecteur TCP
PubSubClient client(espClient); //crée un client MQTT


void callback(char* topic, byte* payload, unsigned int length) { //sert à traiter les messages reçus
  // handle message arrived
}

//--------------Fin partie MQTT----------------//


//_______________Test MQTT________________//
bool reconnect() { // Fonction de reconnexion au broker MQTT
  // Tentative de reconnexion
  if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
    // Une fois connecté, s'abonner au topic avec le nom d'utilisateur et mot de passe
    return true;
  }
  return false;
}

//--------------Fin test MQTT----------------//


//_______________Variables globales________________//
int temperature = 0; // Variable pour stocker la température
long lastReconnectAttempt = 0; // compteur pour la reconnexion


//--------------Fin variables globales----------------//

//_______________Prototypes des fonctions________________//
void connection_maintained(void *pvParameters);
void sensorSimulation(void *pvParameters);
void publishTemperature(void *pvParameters);

//--------------Fin prototypes des fonctions----------------//


void setup() {
  //______________Partie WiFi________________//
  Serial.begin(115200);  // Initialisation de la communication série pour le débogage
  WiFi.begin(ssid, password); // Connexion au réseau WiFi

  Serial.print("Connexion au WiFi");
  while (WiFi.status() != WL_CONNECTED) { // Attente de la connexion
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connecté au WiFi"); // Connexion réussie
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP()); // Affichage de l'adresse IP assignée à

  //--------------Fin partie WiFi----------------//


  //______________Partie MQTT________________//
  client.setServer(mqtt_server, 1883); // Configuration du serveur MQTT sur le port 1883
  client.setCallback(callback); // Définition de la fonction de rappel pour les messages entrants
  lastReconnectAttempt = 0;

  //--------------Fin partie MQTT----------------//

	
  xTaskCreate( connection_maintained, "Task MQTT", 10*configMINIMAL_STACK_SIZE, NULL, 1, NULL );

	xTaskCreate( sensorSimulation, "Task Sensor", 10*configMINIMAL_STACK_SIZE, NULL, 1, NULL );

  xTaskCreate( publishTemperature, "Task Publish", 10*configMINIMAL_STACK_SIZE, NULL, 1, NULL );


}

//_______________reconnection________________//
void connection_maintained(__attribute__((unused)) void *pvParameters) {
  for (;;) {
    if (!client.connected()) {
      long now = millis();
      if (now - lastReconnectAttempt > 5000) {
        lastReconnectAttempt = now;
        if (reconnect()) {
          lastReconnectAttempt = 0;
        }
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    } else {
      client.loop();
      vTaskDelay(1000 / portTICK_PERIOD_MS); // partage du CPU
    }
  }
}
//--------------Fin reconnection----------------//


//_______________émulaation capteur________________//
void sensorSimulation(__attribute__((unused)) void *pvParameters ) {
  for (;;) {
    if (temperature < 50) {
      temperature++;
      Serial.print("Température: ");
      Serial.println(temperature);
    } else {
      temperature = 0;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Attend 1 seconde avant la prochaine incrémentation
  }
}

//--------------Fin émulation capteur----------------//


//_______________envoi température________________//
void publishTemperature(__attribute__((unused)) void *pvParameters) {
  for (;;) {
    client.publish("esp32/bme280/temperature", String(temperature).c_str());
    vTaskDelay(1000 / portTICK_PERIOD_MS); // toutes les 2 secondes
  }
}


//--------------Fin envoi température----------------//

void loop() { // Boucle principale

}

