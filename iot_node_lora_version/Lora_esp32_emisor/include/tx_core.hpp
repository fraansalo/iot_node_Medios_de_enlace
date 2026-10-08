#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include "config.h"

inline void inicializarEmisor(DHT& dht) {
  dht.begin();
  pinMode(WATER_SENSOR_PIN, INPUT);

  SPI.begin(18, 19, 23, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("[ERROR] No se pudo inicializar LoRa en el Emisor.");
    while (1);
  }
  
  // Parámetros de optimización de alcance
  LoRa.setTxPower(20);               
  LoRa.setSpreadingFactor(10);       
  LoRa.setSignalBandwidth(62.5E3);   
  LoRa.setCodingRate4(8);            
  
  Serial.println("[LoRa] Emisor inicializado y optimizado.");
}

inline void adquirirYTransmitirSensores(DHT& dht) {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int valorAguaADC = analogRead(WATER_SENSOR_PIN);
  bool alertaAgua = (valorAguaADC > UMBRAL_AGUA);

  // Construcción del JSON
  StaticJsonDocument<256> doc;
  doc["id_nodo"] = "nodo_01";
  
  if (!isnan(temp) && !isnan(hum)) {
    doc["temperatura_c"] = serialized(String(temp, 1));
    doc["humedad_rh"] = serialized(String(hum, 1));
  } else {
    doc["error_sensor"] = true;
  }
  doc["nivel_agua_adc"] = valorAguaADC;
  doc["alerta_agua"] = alertaAgua;

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  Serial.print("[TX] Enviando JSON por LoRa: ");
  Serial.println(jsonBuffer);

  LoRa.beginPacket();
  LoRa.print(jsonBuffer);
  LoRa.endPacket();
  
  delay(100);
}

inline void entrarEnDeepSleep() {
  Serial.println("[SISTEMA] Entrando en Deep Sleep...");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_M_FACTOR);
  esp_deep_sleep_start();
}