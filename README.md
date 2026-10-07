# Mission 1945 - ESP32 Arcade Shooter Game
**Mission 1945** es un juego de disparos estilo arcade retro desarrollado en C++ para el microcontrolador ESP32, inspirado en los clásicos matamarcianos de 8 bits. El sistema cuenta con renderizado gráfico en pantalla TFT ILI9341, control por Joystick, efectos de sonido pasivos y guardado local de puntuaciones en tarjeta MicroSD.
* **Cristhian Padilla**
* **Octavio Morán**
* **Julián González**
* **Andrés Vázquez**
## Características Principales
* Pantalla TFT ILI9341 (240x320 px): Renderizado gráfico fluido con paleta de colores personalizada (océano, naves y animaciones de combate).
* Controles Integrados: Movimiento mediante Joystick analógico de 2 ejes, botón físico de disparo/acción principal y botón de pausa.
* Sistema de Power-Ups:
  *  Doble Disparo (D): Dispara dos proyectiles paralelos simultáneos.
  *  Escudo Protector (E): Absorbe un impacto directo sin perder vida (con advertencia de parpadeo previo).
  *  Vida Extra (V): Aumenta el contador de vidas (hasta un máximo de 5).
  *  Naves Aliadas (A): Incorpora dos cazas de apoyo laterales con disparo automático.
* Progresión y Batalla de Jefe Final (Boss Barco):
  * 3 Niveles de dificultad progresiva con incremento de velocidad y metas de enemigos derrotados.
  * Enfrentamiento contra un buque jefe militar en 3 fases de combate con puntos débiles destructibles y ráfagas de misiles guiados.
* Audio: Melodías de inicio, Game Over y Victoria generadas mediante buzzer pasivo (tone).
* Almacenamiento Local (MicroSD): Persistencia del Top 5 de mejores puntuaciones guardado en formato JSON
## Esquema de Conexiones de Hardware (ESP32) 
| Periférico / Componente | Pin del ESP32 | Descripción | 
| TFT CS | GPIO 15 | Chip Select de Pantalla TFT ILI9341 | 
| TFT DC | GPIO 2 | Data / Command de Pantalla TFT | 
| SD CS | GPIO 5 | Chip Select de Módulo MicroSD (SPI) | 
| Joystick Eje X | GPIO 34 | Entrada Analógica (Eje X) | 
| Joystick Eje Y | GPIO 35 | Entrada Analógica (Eje Y) | 
| Joystick Botón | GPIO 32 | Botón Pulsador del Joystick | 
| Botón Disparo / Acción | GPIO 13 | Botón de Acción Principal (Pull-Up) | 
| Botón Pausa | GPIO 27 | Botón Físico de Pausa | 
| Buzzer | GPIO 25 | Zumbador Pasivo para Audio / Efectos
## Dependencias y Librerías Requeridas
1. Adafruit_GFX
2. Adafruit_ILI9341
3. ArduinoJson (v6.x)
4. PubSubClient
5. SD (Librería nativa del core de ESP32)
## Estructura del Código
Mission1945/ 
├── Mission1945.ino # Archivo principal con máquina de estados y loop de juego 
├── Config.h # Pines, configuración de Wi-Fi/MQTT y estados del juego 
├── Player.h / .cpp # Control de la nave del jugador, movimiento y power-ups 
├── EnemyManager.h / .cpp # Gestión de enemigos comunes, velocidad y dificultad 
├── Bullet.h / .cpp # Piscina de proyectiles del jugador 
├── PowerUp.h / .cpp # Generación aleatoria y lógica de coleccionables 
├── Boss.h / .cpp # Controlador del Jefe Final (barco), fases y ataques 
└── Storage.h / .cpp # Gestor de lectura/escritura JSON en tarjeta MicroSD
