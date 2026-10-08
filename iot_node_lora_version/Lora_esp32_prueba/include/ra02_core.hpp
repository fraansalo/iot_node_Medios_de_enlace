#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "config.h"

static unsigned long ultimaVezRecibido = 0;
const unsigned long TIMEOUT_ALERTA_MS = 3000; // 5 segundos sin paquetes disparan la alerta
static bool alertaTimeoutEmitida = false;

inline void inicializarRa02() {
  // Configuración de pines específicos para el bus SPI del ESP32
  SPI.begin(18, 19, 23, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("[ERROR] No se pudo inicializar el módulo Ra-02. Revisa las conexiones SPI y alimentación.");
    while (1);
  }
  
  // Opcional: Configurar ganancia y potencia de transmisión máxima
  LoRa.setTxPower(20); // 20 dBm
  LoRa.setSpreadingFactor(10);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.setCodingRate4(8);
  
  Serial.println("[LoRa] Módulo Ra-02 inicializado correctamente a 433 MHz.");
}

inline void enviarPaquetePrueba(int& contador) {
  Serial.print("[TX] Enviando paquete por Ra-02 #");
  Serial.println(contador);

  LoRa.beginPacket();
  LoRa.print("Test Ra-02 Node #");
  LoRa.print(contador);
  LoRa.endPacket();

  contador++;
  delay(2000); // Pausa de 2 segundos entre envíos
}

inline void escucharPaquetes() {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    String mensaje = "";
    while (LoRa.available()) {
      mensaje += (char)LoRa.read();
    }

    int rssi = LoRa.packetRssi();

    Serial.print("[RX] Paquete recibido: \"");
    Serial.print(mensaje);
    Serial.print("\" | RSSI (Señal): ");
    Serial.print(rssi);
    Serial.println(" dBm");

    // Reseteamos el temporizador y el estado de alerta porque llegó un paquete
    ultimaVezRecibido = millis();
    alertaTimeoutEmitida = false;
  } else {
    // Si no hay paquete, verificamos si pasó el tiempo límite desde el último éxito
    if (millis() - ultimaVezRecibido > TIMEOUT_ALERTA_MS) {
      if (!alertaTimeoutEmitida) {
        Serial.println("[ALERTA] No se reciben paquetes LoRa (Timeout de conexión).");
        alertaTimeoutEmitida = true; // Evita que se sature el serial imprimiendo el mensaje en cada ciclo
      }
    }
  }
}