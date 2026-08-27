# 📡 INFORME TÉCNICO
### Monitoreo Ambiental Remoto y Telemetría Industrial
> **Proyecto:** Estación de Telemetría OSP &nbsp;&bull;&nbsp; **Plataforma:** ESP32 & Node-RED &nbsp;&bull;&nbsp; **Versión:** 1.0.0

---

## 1. 📋 Resumen y Objetivos
El presente proyecto implementa una arquitectura de Internet de las Cosas (IoT) orientada al monitoreo ambiental y de seguridad física en tiempo real. El sistema recolecta variables críticas (temperatura, humedad, presencia de agua por inundación y calidad de enlace RF) mediante un nodo de borde basado en **ESP32**, transmitiendo la información de forma segura hacia la nube mediante el protocolo **MQTT sobre TLS (MQTTS)** y visualizándola en un panel de control interactivo (*Dashboard*) desarrollado en **Node-RED**.

> [!NOTE]
> **Bajo Consumo Energético:** Optimización de recursos mediante ciclos de trabajo configurados con *Deep Sleep*, permitiendo una operación autónoma y eficiente del nodo de borde.

> [!IMPORTANT]
> **Seguridad Extremo a Extremo:** Cifrado de transporte TLS v1.2/1.3 operando sobre el puerto estándar `8883`, garantizando la confidencialidad absoluta de la carga útil (*payload*).

---

## 2. ⚡ Profundización en el Protocolo MQTT y el Modelo Pub/Sub
A diferencia de los sistemas tradicionales basados en solicitudes y respuestas (como HTTP), el ecosistema IoT requiere un canal de comunicación ágil que minimice el ancho de banda y el consumo energético. Es aquí donde **MQTT (Message Queuing Telemetry Transport)** se posiciona como el estándar indiscutido de la industria.

### ¿Cómo funciona la arquitectura Publicar/Suscribir (Pub/Sub)?
MQTT prescinde de la conexión directa punto a punto, introduciendo un intermediario centralizado denominado **Broker** (en este desarrollo, *HiveMQ Cloud*). El flujo se rige bajo tres pilares fundamentales:

1. **Publicación (*Publish*):** El nodo de borde (ESP32) adquiere las variables físicas, empaqueta la estructura en formato JSON y la envía al Broker designando una ruta lógica llamada **Tópico** (ej: `telecom/osp/nodo_01`). El microcontrolador no requiere conocer el destino final; cumple con persistir el evento y regresa de inmediato a su estado de suspensión.
2. **Suscripción (*Subscribe*):** Los elementos del sistema interesados en procesar la información (como nuestro servidor Node-RED) se comunican previamente con el Broker para manifestar su interés en escuchar dicho Tópico.
3. **Enrutamiento en Tiempo Real:** Ante la llegada de un nuevo mensaje en un tópico, el Broker reenvía la trama de manera instantánea a todos los clientes que se encuentren suscriptos.

### ¿Qué beneficios aporta este modelo?
* **Desacoplamiento Temporal y Espacial:** El emisor y el receptor no precisan operar de manera simultánea. Si el servidor de visualización experimenta una interrupción temporal, el Broker gestiona la sincronización de los nuevos estados en cuanto el servicio retorna a línea.

* **Escalabilidad por Jerarquías:** Mediante la estructuración de tópicos con barras de directorios (ej: `planta/sector_a/nodo_temp`), es posible escalar a flotas masivas de dispositivos sin alterar la lógica central del sistema.

---

## 3. 🛠️ Arquitectura del Sistema y Topología
La solución se desglosa en tres capas funcionales que garantizan la modularidad y el desacoplamiento operativo:

| Capa del Sistema | Componente Tecnológico | Función Principal en la Solución |
| :--- | :--- | :--- |
| **Capa de Percepción** | ESP32 + DHT11 + Sensor de Agua | Adquisición física de variables analógicas/digitales y empaquetado estructurado en JSON. |
| **Capa de Transporte** | HiveMQ Cloud (Broker MQTT) | Enrutamiento de mensajes bajo modelo Publish/Subscribe con seguridad TLS integrada. |
| **Capa de Aplicación** | Node-RED + Dashboard (Chart.js) | Procesamiento de tramas, lógica de alertas críticas y renderizado de la interfaz gráfica. |

---

## 4. 🧩 Diseño del Firmware y Modularización
El código fuente en C++ para el ESP32 se estructuró bajo estándares de ingeniería de software utilizando **PlatformIO**, aislando la lógica de negocio y las configuraciones de hardware en módulos independientes:

* **`config.h`:** Cabecera destinada al almacenamiento centralizado de credenciales de red, parámetros de conexión al Broker y asignación de pines físicos mediante macros eficientes (`#define`).
* **`nodo_core.hpp`:** Módulo funcional que encapsula la rutina de conexión segura, gestión de sockets y la serialización dinámica del documento JSON mediante la librería *ArduinoJson*. Se implementó la directiva `inline` para prevenir errores de definiciones múltiples en el enlazador (*linker*).
* **`main.cpp`:** Archivo principal que opera como director de orquestación, estructurando el ciclo de vida del sistema en inicialización, adquisición, transmisión y entrada controlada a modo de suspensión profunda (*Deep Sleep*).

---

## 5. 🚀 Conclusiones y Escalabilidad
El desarrollo demuestra una alta versatilidad de despliegue. Puede implementarse de forma inmediata en entornos académicos mediante túneles seguros locales (**Ngrok**), o bien evolucionar de manera transparente hacia una arquitectura industrial 100% cloud migrando el motor de procesamiento Node-RED a un servidor virtual privado (VPS) con disponibilidad continua 24/7 sin incurrir en costos operativos adicionales.