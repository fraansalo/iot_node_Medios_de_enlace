#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "config.h"
#include "rx_core.hpp"

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(100);

  conectarWiFi();
  conectarMQTT(client, espClient);
  inicializarReceptorLoRa();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }
  if (!client.connected()) {
    conectarMQTT(client, espClient);
  }
  
  client.loop();
  procesarRecepciónYReenvioMQTT(client);
}