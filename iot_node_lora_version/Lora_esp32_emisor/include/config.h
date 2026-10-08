#pragma once

// --- PINES DE SENSORES ---
#define DHTPIN 4
#define DHTTYPE DHT11
#define WATER_SENSOR_PIN 32
#define UMBRAL_AGUA 500

// --- CONFIGURACIÓN LORA (Optimizada largo alcance) ---
#define LORA_FREQ 433E6 
#define LORA_SS   15
#define LORA_RST  4
#define LORA_DIO0 2

// --- GESTIÓN ENERGÉTICA ---
#define TIME_TO_SLEEP 15     // Segundos entre transmisiones
#define uS_TO_M_FACTOR 1000000ULL