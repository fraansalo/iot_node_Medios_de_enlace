#pragma once

// --- CREDENCIALES DE RED Y CLOUD ---
#define WIFI_SSID "Tu_Red_WiFi"
#define WIFI_PASS "Tu_Contraseña_WiFi"
#define MQTT_SERVER "tu-servidor.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "admin"
#define MQTT_PASS "tu_password_mqtt"
#define MQTT_TOPIC "telecom/osp/nodo_01"

// --- CONFIGURACIÓN LORA (Debe coincidir exactamente con el emisor) ---
#define LORA_FREQ 433E6 
#define LORA_SS   15
#define LORA_RST  4
#define LORA_DIO0 2
