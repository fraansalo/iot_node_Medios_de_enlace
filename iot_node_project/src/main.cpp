#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// 1. CREDENCIALES
const char* ssid = "TU_WIFI_O_HOTSPOT";
const char* password = "TU_PASSWORD_WIFI";

const char* mqtt_server = "xxxxxx.s1.eu.hivemq.cloud"; 
const int mqtt_port = 8883;                             
const char* mqtt_user = "nodo_osp";                     
const char* mqtt_pass = "Clave_1234";                   

const char* mqtt_topic = "telecom/osp/nodo_01";

// 2. CONFIGURACIÓN DE HARDWARE
#define DHTPIN 4             // Pin GPIO4 conectado al pin DATA del DHT11
#define DHTTYPE DHT11        // Definición del modelo exacto de sensor
#define WATER_SENSOR_PIN 32  // Pin GPIO32 para el sensor de agua
#define TIME_TO_SLEEP 10     // Minutos en suspensión profunda
#define uS_TO_M_FACTOR 60000000ULL 

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(100);
  
  pinMode(WATER_SENSOR_PIN, INPUT_PULLDOWN);
  dht.begin(); // Inicializa el bus One-Wire del DHT11

  // 3. CONEXIÓN WI-FI (CAPA DE ENLACE / RED)
  WiFi.begin(ssid, password);
  Serial.print("[Wi-Fi] Conectando");
  
  int wifi_timeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_timeout < 20) {
    delay(500);
    Serial.print(".");
    wifi_timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[Wi-Fi] Conectado con éxito");

    // Configuración de cifrado TLS
    espClient.setInsecure(); 
    client.setServer(mqtt_server, mqtt_port);

    // 4. CONEXIÓN MQTT (CAPA DE APLICACIÓN)
    Serial.print("[MQTT] Conectando a HiveMQ Cloud...");
    String clientId = "ESP32_OSP_";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("\n[MQTT] Conexión Exitosa");

      // 5. LECTURA DE SENSORES Y CAPA FÍSICA
      float temp = dht.readTemperature();
      float hum = dht.readHumidity();
      bool agua = digitalRead(WATER_SENSOR_PIN);
      long rssi = WiFi.RSSI();

      // Construcción del JSON
      StaticJsonDocument<256> doc;
      doc["id_nodo"] = "nodo_01";
      
      if (!isnan(temp) && !isnan(hum)) {
        doc["temperatura_c"] = serialized(String(temp, 1));
        doc["humedad_rh"] = serialized(String(hum, 1));
      } else {
        doc["error_sensor"] = true;
      }
      doc["alerta_agua"] = agua;
      doc["rssi_dbm"] = rssi;

      char jsonBuffer[256];
      serializeJson(doc, jsonBuffer);

      Serial.print("[MQTT] Enviando Payload: ");
      Serial.println(jsonBuffer);

      if (client.publish(mqtt_topic, jsonBuffer)) {
        Serial.println("[MQTT] Mensaje publicado correctamente");
      } else {
        Serial.println("[ERROR] Fallo al publicar");
      }

      delay(200);
      client.disconnect();
    } else {
      Serial.print("\n[ERROR] Falló la conexión MQTT. Estado: ");
      Serial.println(client.state());
    }
  } else {
    Serial.println("\n[ERROR] No se pudo conectar a la red Wi-Fi");
  }

  // 6. SUSPENSIÓN PROFUNDA (DEEP SLEEP)
  Serial.println("[SISTEMA] Entrando en Deep Sleep...");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_M_FACTOR);
  esp_deep_sleep_start();
}

void loop() {
  // Sin código. El sistema se reinicia desde setup() al despertar.
}