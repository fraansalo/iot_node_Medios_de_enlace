#pragma once

// --- CREDENCIALES DE RED ---
#define WIFI_SSID "TeleCentro-7933"
#define WIFI_PASS "NDZZQGZXKTNC"
#define MQTT_SERVER "78e28dbcec28442a938b2ba8b8699a94.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "admin"
#define MQTT_PASS "nodo_iot"
#define MQTT_TOPIC "telecom/osp/nodo_01"

// --- CONFIGURACIÓN DE HARDWARE ---
#define DHTPIN 4
#define DHTTYPE DHT11
#define WATER_SENSOR_PIN 32
#define UMBRAL_AGUA 500

// --- ENERGÍA ---
#define TIME_TO_SLEEP 15     
#define uS_TO_M_FACTOR 1000000ULL