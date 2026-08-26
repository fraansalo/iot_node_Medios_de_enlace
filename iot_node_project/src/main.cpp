#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "config.h"
#include "nodo_core.hpp"

// Instanciación global de objetos
DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(100);
  
  dht.begin();
  pinMode(WATER_SENSOR_PIN, INPUT);

  conectarWiFi();
  
  if (WiFi.status() == WL_CONNECTED) {
    if (conectarMQTT(client, espClient)) {
      recolectarYPublicar(client, dht);
    }
  }
  
  entrarDeepSleep();
}

void loop() {
  // Vacío por ciclo de Deep Sleep
}