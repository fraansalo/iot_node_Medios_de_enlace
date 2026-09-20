/*#include <Arduino.h>
#include "config.h"
#include "ra02_core.hpp"

int contador = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  inicializarRa02();
  Serial.println("[MODO] ESP32 configurado como TRANSMISOR (TX)");
}

void loop() {
  enviarPaquetePrueba(contador);
}*/