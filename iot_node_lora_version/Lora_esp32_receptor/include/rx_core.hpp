#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LoRa.h>
#include "config.h"

static unsigned long ultimaVezRecibido = 0;
const unsigned long TIMEOUT_ALERTA_MS = 10000; // 10 segundos de tolerancia sin datos
static bool alertaTimeoutEmitida = false;

inline void conectarWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("[Wi-Fi] Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[Wi-Fi] Conectado exitosamente.");
}

inline void conectarMQTT(PubSubClient& client, WiFiClientSecure& espClient) {
  espClient.setInsecure(); // Omitir validación estricta para pruebas con certificado cloud
  client.setServer(MQTT_SERVER, MQTT_PORT);

  while (!client.connected()) {
    String clientId = "ESP32_Gateway_" + String(random(0xffff), HEX);
    Serial.print("[MQTT] Conectando al Broker...");
    
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println(" ¡Conectado!");
    } else {
      Serial.print(" Falló, rc=");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

inline void inicializarReceptorLoRa() {
  SPI.begin(18, 19, 23, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("[ERROR] No se pudo inicializar LoRa en el Receptor.");
    while (1);
  }
  
  // Mismos parámetros de optimización para asegurar compatibilidad de radio
  LoRa.setTxPower(20);               
  LoRa.setSpreadingFactor(10);       
  LoRa.setSignalBandwidth(62.5E3);   
  LoRa.setCodingRate4(8);            
  
  Serial.println("[LoRa] Receptor inicializado y listo para escuchar.");
  ultimaVezRecibido = millis();
}

inline void procesarRecepciónYReenvioMQTT(PubSubClient& client) {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    String payloadLoRa = "";
    while (LoRa.available()) {
      payloadLoRa += (char)LoRa.read();
    }

    int rssiLoRa = LoRa.packetRssi();
    Serial.print("[LoRa RX] Trama recibida. RSSI: ");
    Serial.print(rssiLoRa);
    Serial.print(" dBm | Payload: ");
    Serial.println(payloadLoRa);

    // Reenvío automático hacia HiveMQ Cloud (Node-RED)
    if (client.publish(MQTT_TOPIC, payloadLoRa.c_str())) {
      Serial.println("[MQTT] Reenvío a la nube exitoso.");
    } else {
      Serial.println("[ERROR] Falló el reenvío MQTT a HiveMQ.");
    }

    // Resetear timeout
    ultimaVezRecibido = millis();
    alertaTimeoutEmitida = false;
  } else {
    // Control de tiempo sin recibir paquetes del emisor
    if (millis() - ultimaVezRecibido > TIMEOUT_ALERTA_MS) {
      if (!alertaTimeoutEmitida) {
        Serial.println("[ALERTA] Timeout: No se reciben datos LoRa del emisor.");
        alertaTimeoutEmitida = true;
      }
    }
  }
}