#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// --- CONFIGURACIÓN DE RED Y HIVEMQ MQTT ---
// ============================================================================
#define WIFI_SSID "TU_NOMBRE_DE_WIFI"
#define WIFI_PASSWORD "TU_CONTRASEÑA_WIFI"
#define MQTT_BROKER "tu-cluster.hivemq.cloud" // Reemplaza con tu Host de HiveMQ
#define MQTT_PORT 8883 // Puerto SSL/TLS de HiveMQ Cloud
#define MQTT_USER "tu_usuario_hivemq"
#define MQTT_PASS "tu_password_hivemq"
#define MQTT_TOPIC "mision1945/metricas"

// ============================================================================
// --- ASIGNACIÓN DE PINES HARDWARE ESP32 ---
// ============================================================================
#define TFT_CS 15 // Chip Select Pantalla TFT ILI9341
#define TFT_DC 2 // Data/Command Pantalla TFT
#define SD_CS 5 // Chip Select Tarjeta MicroSD (SPI)
#define PIN_PAUSA 27 // Botón físico de Pausa
#define PIN_BTN 13 // Botón físico externo de Disparo / Selección (GPIO 13)
#define JOY_X 34 // Eje X Joystick (Entrada Analógica)
#define JOY_Y 35 // Eje Y Joystick (Entrada Analógica)
#define JOY_BTN 32 // Botón Pulsador del Joystick
#define PIN_BUZZER 25 // Zumbador / Buzzer Pasivo
#define PIN_MOTOR 26 // Motor de Vibración Hápitco

// ============================================================================
// --- ESTADOS DEL JUEGO ---
// ============================================================================
enum EstadoJuego { INTRO, MENU, JUEGO, PAUSA, SCORES, GAMEOVER, LEVEL_UP, VICTORIA };
#ifndef COLOR_MAR 
#define COLOR_MAR ILI9341_NAVY // Azul marino para el océano 
#endif
#endif