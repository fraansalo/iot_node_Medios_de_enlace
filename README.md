<div style="background: linear-gradient(135deg, #1e1e2f 0%, #2a2a40 100%); padding: 30px; border-radius: 15px; color: white; text-align: center; margin-bottom: 30px;">
    <h1 style="margin: 0; font-size: 28px; color: #00e5ff;">INFORME TÉCNICO DE SISTEMA IoT</h1>
    <h3 style="margin: 10px 0 0 0; color: #a0a0a0; font-weight: normal;">Monitoreo Ambiental Remoto y Telemetría Industrial</h3>
    <hr style="border: 0; height: 1px; background: rgba(255,255,255,0.2); margin: 20px 0;">
    <p style="margin: 0; font-size: 14px;"><strong>Autor:</strong> Estación de Telemetría OSP | <strong>Plataforma:</strong> ESP32 & Node-RED</p>
</div>

<h2>1. Resumen Ejecutivo y Objetivos</h2>
<p>El presente proyecto implementa una arquitectura de Internet de las Cosas (IoT) orientada al monitoreo ambiental y de seguridad física en tiempo real. El sistema recolecta variables críticas (temperatura, humedad, presencia de agua por inundación y calidad de enlace RF) mediante un nodo de borde basado en <strong>ESP32</strong>, transmitiendo la información de forma segura hacia la nube mediante el protocolo <strong>MQTT sobre TLS (MQTTS)</strong> y visualizándola en un panel de control interactivo (Dashboard) desarrollado en <strong>Node-RED</strong>.</p>

<div style="display: flex; gap: 15px; margin: 20px 0;">
    <div style="flex: 1; background: #f8f9fa; border-left: 5px solid #4CAF50; padding: 15px; border-radius: 4px; color: #333;">
        <h4 style="margin: 0 0 5px 0; color: #2e7d32;">Bajo Consumo</h4>
        <p style="margin: 0; font-size: 13px;">Optimización energética mediante ciclos de trabajo con <strong>Deep Sleep</strong> para operación autónoma.</p>
    </div>
    <div style="flex: 1; background: #f8f9fa; border-left: 5px solid #2196F3; padding: 15px; border-radius: 4px; color: #333;">
        <h4 style="margin: 0 0 5px 0; color: #1565C0;">Seguridad Extremo a Extremo</h4>
        <p style="margin: 0; font-size: 13px;">Cifrado TLS v1.2/1.3 en puerto 8883 garantizando la confidencialidad de la carga útil.</p>
    </div>
</div>

<hr style="margin: 30px 0;">

<h2>2. Arquitectura del Sistema y Topología</h2>
<p>La solución se divide en tres capas principales que desacoplan la adquisición de datos, el transporte seguro y la capa de presentación:</p>

<table style="width: 100%; border-collapse: collapse; margin-top: 15px; font-size: 14px;">
    <thead>
        <tr style="background-color: #2a2a40; color: white;">
            <th style="padding: 10px; text-align: left; border: 1px solid #ddd;">Capa</th>
            <th style="padding: 10px; text-align: left; border: 1px solid #ddd;">Componente</th>
            <th style="padding: 10px; text-align: left; border: 1px solid #ddd;">Función Principal</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td style="padding: 10px; border: 1px solid #ddd;"><strong>Capa de Percepción</strong></td>
            <td style="padding: 10px; border: 1px solid #ddd;">ESP32 + DHT11 + Sensor de Agua</td>
            <td style="padding: 10px; border: 1px solid #ddd;">Adquisición física de variables analógicas y digitales, empaquetado JSON.</td>
        </tr>
        <tr style="background-color: #f9f9f9;">
            <td style="padding: 10px; border: 1px solid #ddd;"><strong>Capa de Transporte</strong></td>
            <td style="padding: 10px; border: 1px solid #ddd;">HiveMQ Cloud (Broker MQTT)</td>
            <td style="padding: 10px; border: 1px solid #ddd;">Enrutamiento de mensajes bajo modelo Publish/Subscribe con seguridad TLS.</td>
        </tr>
        <tr>
            <td style="padding: 10px; border: 1px solid #ddd;"><strong>Capa de Aplicación</strong></td>
            <td style="padding: 10px; border: 1px solid #ddd;">Node-RED + Dashboard (Chart.js)</td>
            <td style="padding: 10px; border: 1px solid #ddd;">Procesamiento de tramas, lógica de alertas y renderizado de interfaz gráfica.</td>
        </tr>
    </tbody>
</table>

<hr style="margin: 30px 0;">

<h2>3. Protocolos de Comunicación Utilizados</h2>
<ul>
    <li><strong>MQTT (Message Queuing Telemetry Transport):</strong> Elegido frente a HTTP por su extrema ligereza, bajo overhead de cabecera (2 bytes mínimos) y su arquitectura basada en eventos (Pub/Sub). Ideal para redes inestables o dispositivos con restricciones energéticas.</li>
    <li><strong>TLS (Transport Layer Security):</strong> Implementado sobre el puerto <code>8883</code>. Asegura que la comunicación entre el microcontrolador y el broker en la nube no pueda ser interceptada ni modificada por ataques de hombre en el medio (MitM).</li>
    <li><strong>JSON (JavaScript Object Notation):</strong> Formato estándar de intercambio de datos utilizado para estructurar las cargas útiles (*payloads*).</li>
</ul>

<hr style="margin: 30px 0;">

<h2>4. Diseño del Firmware y Modularización</h2>
<p>El código fuente en C++ para el ESP32 se estructuró bajo buenas prácticas de ingeniería de software utilizando <strong>PlatformIO</strong>, separando las configuraciones de hardware y la lógica de negocio en cabeceras modulares:</p>

<ul>
    <li><code>config.h</code>: Archivo de cabecera dedicado al almacenamiento centralizado de credenciales Wi-Fi, parámetros del broker MQTT y pines de hardware mediante macros eficientes (<code>#define</code>).</li>
    <li><code>nodo_core.hpp</code>: Módulo que encapsula las funciones de conectividad, manejo de sockets seguros y la construcción dinámica del documento JSON mediante <em>ArduinoJson</em>. Se empleó la directiva <code>inline</code> para evitar conflictos de enlaces múltiples (*linker errors*).</li>
    <li><code>main.cpp</code>: Archivo principal que opera como director de orquestación, estructurando el ciclo de vida del microcontrolador en adquisición de datos, publicación y entrada inmediata a modo de suspensión profunda (<em>Deep Sleep</em>).</li>
</ul>

<hr style="margin: 30px 0;">

<h2>5. Conclusiones y Escalabilidad</h2>
<p>El sistema desarrollado demuestra una alta versatilidad: puede desplegarse de manera inmediata en entornos académicos mediante túneles seguros locales (Ngrok), o bien escalar hacia una arquitectura industrial 100% cloud migrando el motor de procesamiento Node-RED a un servidor virtual privado (VPS) con operación continua 24/7 sin incurrir en costos operativos.</p>