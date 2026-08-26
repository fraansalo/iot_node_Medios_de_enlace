#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define WATER_SENSOR_PIN 32  // Canal ADC1_CH4

// Umbral empírico para ESP32 (Rango 0 a 4095)
// Ajustar según pruebas: en seco suele dar < 300, con humedad/agua sube de 500 a 3000+
const int UMBRAL_AGUA = 500; 

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Configuración del pin analógico en ESP32
  pinMode(WATER_SENSOR_PIN, INPUT);
  
  Serial.println("\n[SISTEMA] Iniciando test de capa física (ADC)...");
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  
  // Lectura analógica directa del ADC
  int valorAguaADC = analogRead(WATER_SENSOR_PIN);
  bool alertaAgua = (valorAguaADC > UMBRAL_AGUA);

  if (isnan(temp) || isnan(hum)) {
    Serial.println("[ERROR] Falla en bus de datos del DHT11");
  } else {
    Serial.printf("Temp: %.1f C | Hum: %.1f %% | Nivel Agua (ADC): %d | Alerta: %s\n", 
                  temp, hum, valorAguaADC, alertaAgua ? "SI" : "NO");
  }
  
  delay(1500);
}