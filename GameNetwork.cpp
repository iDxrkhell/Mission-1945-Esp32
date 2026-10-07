#include "GameNetwork.h"

GameNetwork::GameNetwork(const char* wifiSSID, const char* wifiPass, const char* broker, int port, const char* user, const char* pass, const char* topic) : mqttClient(espClient) {
    ssid = wifiSSID;
    password = wifiPass;
    mqttBroker = broker;
    mqttPort = port;
    mqttUser = user;
    mqttPass = pass;
    mqttTopic = topic;
}

void GameNetwork::iniciarConexion() {
    Serial.print("Conectando a Wi-Fi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    int intentos = 0;
    while (WiFi.status() != WL_CONNECTED && intentos < 5) {
        delay(500);
        Serial.print(".");
        intentos++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi Conectado con exito!");
        Serial.print("IP asignada: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nError: No se pudo conectar a Wi-Fi (Modo Offline activo)");
    }

    // Configuración SSL/TLS sin validación estricta de certificado CA para HiveMQ Cloud
    espClient.setInsecure();
    mqttClient.setServer(mqttBroker, mqttPort);
}

void GameNetwork::reconectarMQTT() {
    if (WiFi.status() != WL_CONNECTED) return;
    Serial.print("Intentando conexion MQTT a HiveMQ...");
    String clientID = "ESP32_Mission1945_" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientID.c_str(), mqttUser, mqttPass)) {
        Serial.println(" Conectado!");
    } else {
        Serial.print(" Fallo la conexion. Estado error: ");
        Serial.println(mqttClient.state());
    }
}

void GameNetwork::actualizar() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!mqttClient.connected()) {
            reconectarMQTT();
        }
        mqttClient.loop();
    }
}

void GameNetwork::enviarMetricasMQTT(const char* resultado, int score, int nivel, int enemigosDerrotados, int vidas) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[MQTT] Sin Wi-Fi. No se enviaron metricas.");
        return;
    }
    if (!mqttClient.connected()) {
        reconectarMQTT();
    }
    if (mqttClient.connected()) {
        StaticJsonDocument<256> doc;
        doc["juego"] = "Mission 1945";
        doc["resultado"] = resultado;
        doc["score"] = score;
        doc["nivel_alcanzado"] = nivel;
        doc["enemigos_derrotados"] = enemigosDerrotados;
        doc["vidas_restantes"] = vidas;
        char buffer[256];
        serializeJson(doc, buffer);
        if (mqttClient.publish(mqttTopic, buffer)) {
            Serial.print("[MQTT] Metricas enviadas a HiveMQ: ");
            Serial.println(buffer);
        } else {
            Serial.println("[MQTT] Error al publicar el mensaje.");
        }
    } else {
        Serial.println("[MQTT] No conectado a HiveMQ. No se enviaron metricas.");
    }
}