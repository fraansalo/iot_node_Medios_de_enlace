#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "config.h"

inline void conectarWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("[Wi-Fi] Conectando");
  
  int wifi_timeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_timeout < 20) {
    delay(500);
    Serial.print(".");
    wifi_timeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[Wi-Fi] Conectado");
  } else {
    Serial.println("\n[ERROR] Falló la conexión Wi-Fi");
  }
}

inline bool conectarMQTT(PubSubClient& client, WiFiClientSecure& espClient) {
  espClient.setInsecure(); 
  client.setServer(MQTT_SERVER, MQTT_PORT);

  String clientId = "ESP32_OSP_" + String(random(0xffff), HEX);

  if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
    Serial.println("[MQTT] Conexión Exitosa");
    return true;
  }
  
  Serial.println("[ERROR] Falla en conexión MQTT");
  return false;
}

inline void recolectarYPublicar(PubSubClient& client, DHT& dht) {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int valorAguaADC = analogRead(WATER_SENSOR_PIN);
  bool alertaAgua = (valorAguaADC > UMBRAL_AGUA);
  long rssi = WiFi.RSSI();

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
  doc["rssi_dbm"] = rssi;

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  Serial.print("[MQTT] Payload: ");
  Serial.println(jsonBuffer);

  if (client.publish(MQTT_TOPIC, jsonBuffer)) {
    Serial.println("[MQTT] Publicación exitosa");
  } else {
    Serial.println("[ERROR] Falló la publicación");
  }

  client.loop(); 
  delay(500); 
  client.disconnect();
}

inline void entrarDeepSleep() {
  Serial.println("[SISTEMA] Deep Sleep...");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_M_FACTOR);
  esp_deep_sleep_start();
}