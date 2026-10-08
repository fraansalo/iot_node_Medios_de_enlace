#include <Arduino.h>
#include <DHT.h>
#include "config.h"
#include "tx_core.hpp"

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(100);

  inicializarEmisor(dht);
  adquirirYTransmitirSensores(dht);
  entrarEnDeepSleep();
}

void loop() {
  // Vacío por Deep Sleep
}