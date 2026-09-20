#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "config.h"

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
  }
}