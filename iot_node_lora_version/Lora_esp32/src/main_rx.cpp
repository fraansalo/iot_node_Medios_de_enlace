#include <Arduino.h>
#include "config.h"
#include "ra02_core.hpp"

void setup() {
  Serial.begin(115200);
  delay(100);
  
  inicializarRa02();
  Serial.println("[MODO] ESP32 configurado como RECEPTOR (RX)");
}

void loop() {
  escucharPaquetes();
}