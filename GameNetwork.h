#ifndef GAME_NETWORK_H
#define GAME_NETWORK_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class GameNetwork {
private:
    const char* ssid;
    const char* password;
    const char* mqttBroker;
    int mqttPort;
    const char* mqttUser;
    const char* mqttPass;
    const char* mqttTopic;
    WiFiClientSecure espClient;
    PubSubClient mqttClient;

    void reconectarMQTT();

public:
    GameNetwork(const char* wifiSSID, const char* wifiPass, const char* broker, int port, const char* user, const char* pass, const char* topic);

    // Inicia la conexión a Wi-Fi y configura el cliente SSL de HiveMQ
    void iniciarConexion();

    // Mantiene viva la sesión MQTT en el loop principal
    void actualizar();

    // Transmite el JSON de métricas al terminar la partida (GAMEOVER o VICTORIA)
    void enviarMetricasMQTT(const char* resultado, int score, int nivel, int enemigosDerrotados, int vidas);

    // Devuelve el estado de conexión Wi-Fi
    bool estaConectado() { return WiFi.status() == WL_CONNECTED; }
};

#endif