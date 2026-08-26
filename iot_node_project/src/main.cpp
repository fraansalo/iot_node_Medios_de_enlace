#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// --- CREDENCIALES DE RED ---
const char* ssid = "TeleCentro-7933";
const char* password = "NDZZQGZXKTNC";
const char* mqtt_server = "78e28dbcec28442a938b2ba8b8699a94.s1.eu.hivemq.cloud"; 
const int mqtt_port = 8883;                             
const char* mqtt_user = "admin";                     
const char* mqtt_pass = "nodo_iot";                   
const char* mqtt_topic = "telecom/osp/nodo_01";

// --- CONFIGURACIÓN DE HARDWARE ---
#define DHTPIN 4
#define DHTTYPE DHT11
#define WATER_SENSOR_PIN 32
const int UMBRAL_AGUA = 500;
#define TIME_TO_SLEEP 15     
// #define uS_TO_M_FACTOR 60000000ULL  conversor para minutos
#define uS_TO_M_FACTOR 1000000ULL
DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(100);
  
  dht.begin();
  pinMode(WATER_SENSOR_PIN, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("[Wi-Fi] Conectando");
  
  int wifi_timeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_timeout < 20) {
    delay(500);
    Serial.print(".");
    wifi_timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[Wi-Fi] Conectado");
    
    espClient.setInsecure(); 
    client.setServer(mqtt_server, mqtt_port);

    String clientId = "ESP32_OSP_";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("[MQTT] Conexión Exitosa");

      // Adquisición de datos
      float temp = dht.readTemperature();
      float hum = dht.readHumidity();
      int valorAguaADC = analogRead(WATER_SENSOR_PIN);
      bool alertaAgua = (valorAguaADC > UMBRAL_AGUA);
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
      doc["nivel_agua_adc"] = valorAguaADC;
      doc["alerta_agua"] = alertaAgua;
      doc["rssi_dbm"] = rssi;

      char jsonBuffer[256];
      serializeJson(doc, jsonBuffer);

      Serial.print("[MQTT] Payload: ");
      Serial.println(jsonBuffer);

      if (client.publish(mqtt_topic, jsonBuffer)) {
        Serial.println("[MQTT] Publicación exitosa");
      } else {
        Serial.println("[ERROR] Falló la publicación");
      }

      // Procesa eventos pendientes del socket TCP para asegurar el envío de la trama
      client.loop(); 
      delay(500); 

      client.disconnect();
      // --- FIN DEL AJUSTE ---
    }
  }

  Serial.println("[SISTEMA] Deep Sleep...");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_M_FACTOR);
  esp_deep_sleep_start();
}

void loop() {}