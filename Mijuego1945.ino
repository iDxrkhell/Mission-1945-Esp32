#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "Player.h"
#include "EnemyManager.h"
#include "Bullet.h"
#include "PowerUp.h"
#include "Boss.h"
#include "GameNetwork.h"
#include "Storage.h"

// ============================================================================
// --- INSTANCIAS DE MÓDULOS Y OBJETOS ---
// ============================================================================
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, -1);
Player jugador;
EnemyManager enemigos;
BulletManager balas;
PowerUp powerUp;
Boss boss;
GameNetwork red(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER, MQTT_PORT, MQTT_USER, MQTT_PASS, MQTT_TOPIC);
StorageManager almacenamiento(SD_CS);

// ============================================================================
// --- VARIABLES GLOBALES DE ESTADO ---
// ============================================================================
EstadoJuego estado = INTRO;
int score = 0;
int nivel = 1;
int topScores[5] = { 0, 0, 0, 0, 0 };
bool introDibujada = false;
// Temporizadores de Audio y Control
unsigned long ultimoDisparo = 0;
unsigned long ultimoTono = 0;
bool notaAlta = true;
int menuSeleccion = 0;  // 0 = Iniciar, 1 = Scores
bool botonPausaPrevio = HIGH;
bool botonJoyPrevio = HIGH;
bool botonAccionPrevio = HIGH;

// Declara la función de resumen para el Monitor Serial
void imprimirResumenSerial(const char* resultado);

// ============================================================================
// --- FUNCIONES DE AUDIO ---
// ============================================================================
void reproducirMelodiaIntro() {
  int melodia[] = { 392, 523, 587, 784, 587, 784 };
  int duraciones[] = { 150, 150, 150, 280, 120, 350 };
  for (int i = 0; i < 6; i++) {
    tone(PIN_BUZZER, melodia[i], duraciones[i]);
    delay(duraciones[i] + 50);
  }
}
void reproducirMelodiaGameOver() {
  int melodia[] = { 392, 370, 349, 330, 311, 294, 262 };
  int duraciones[] = { 200, 200, 200, 200, 250, 250, 600 };
  for (int i = 0; i < 7; i++) {
    tone(PIN_BUZZER, melodia[i], duraciones[i]);
    delay(duraciones[i] + 40);
  }
  noTone(PIN_BUZZER);
}
void reproducirMelodiaVictoria() {
  int melodia[] = { 523, 523, 523, 659, 784, 1047 };
  int duraciones[] = { 100, 100, 100, 250, 250, 600 };
  for (int i = 0; i < 6; i++) {
    tone(PIN_BUZZER, melodia[i], duraciones[i]);
    delay(duraciones[i] + 30);
  }
  noTone(PIN_BUZZER);
}
void sonidoMenu() {
  tone(PIN_BUZZER, 600, 80);
}

void sonidoDisparo() {
  tone(PIN_BUZZER, 850, 50);
}

void sonidoImpacto() {
  tone(PIN_BUZZER, 150, 150);
}

// ============================================================================
// --- REINICIO Y RESTAURACIÓN DE PARTIDA ---
// ============================================================================
void resetearJuego() {
  score = 0;
  nivel = 1;
  jugador.reset();
  enemigos.reset();
  enemigos.configurarNivel(1);
  balas.reset();
  powerUp.reset();
  boss.reset();  // Restauración del Boss a BOSS_INACTIVO, Fase 1 y despenalización de proyectiles
}

// ============================================================================
// --- ANIMACIÓN DE BOMBA ATÓMICA (VICTORIA) ---
// ============================================================================
void animacionBombaAtomica() {
  int centroX = 120;
  int centroY = 160;

  // --- FASE 1: DESTELLO INICIAL BLANCO Y VIBRACIÓN ---
  digitalWrite(PIN_MOTOR, HIGH);
  tone(PIN_BUZZER, 1200, 300);
  tft.fillScreen(ILI9341_WHITE);
  delay(150);
  tft.fillScreen(ILI9341_YELLOW);
  delay(100);

  // --- FASE 2: BOLA DE FUEGO INCANDESCENTE EXPANDIÉNDOSE ---
  for (int r = 10; r <= 140; r += 8) {
    // Círculos concéntricos (Centro Blanco, Fuego Amarillo, Borde Naranja/Rojo)
    tft.fillCircle(centroX, centroY, r, ILI9341_ORANGE);
    tft.fillCircle(centroX, centroY, (int)(r * 0.8), ILI9341_YELLOW);
    tft.fillCircle(centroX, centroY, (int)(r * 0.5), ILI9341_WHITE);

    // Sonido de estruendo variando frecuencia
    tone(PIN_BUZZER, random(100, 300), 30);
    delay(20);
  }
  digitalWrite(PIN_MOTOR, LOW);

  // --- FASE 3: ONDA DE CHOQUE EXPANDIÉNDOSE A LOS BORDES ---
  for (int r = 10; r <= 180; r += 12) {
    // Anillo de choque Cyan/Blanco que borra el centro a Negro
    tft.drawCircle(centroX, centroY, r, ILI9341_CYAN);
    tft.drawCircle(centroX, centroY, r - 1, ILI9341_WHITE);
    tft.drawCircle(centroX, centroY, r - 2, ILI9341_CYAN);
    if (r > 30) {
      tft.drawCircle(centroX, centroY, r - 25, ILI9341_BLACK);
    }
    delay(15);
  }

  // --- FASE 4: EFECTO HONGO NUCLEAR Y DESTELLOS DE ESCOMBROS ---
  tft.fillScreen(ILI9341_BLACK);

  // Dibujar estructura del hongo visto desde arriba
  for (int i = 0; i < 20; i++) {
    // Nube de humo externa (Gris Oscuro)
    int offsetX = random(-30, 30);
    int offsetY = random(-30, 30);
    int radioHumo = random(30, 60);
    tft.fillCircle(centroX + offsetX, centroY + offsetY, radioHumo, ILI9341_DARKGREY);

    // Núcleo caliente interno (Rojo y Naranja)
    int radioFuego = random(10, 25);
    tft.fillCircle(centroX + (offsetX / 2), centroY + (offsetY / 2), radioFuego, ILI9341_RED);

    // Chispas/Partículas proyectadas hacia afuera
    for (int p = 0; p < 5; p++) {
      int px = centroX + random(-100, 100);
      int py = centroY + random(-130, 130);
      tft.fillCircle(px, py, random(1, 3), ILI9341_YELLOW);
    }

    // Tono decreciente de explosión
    tone(PIN_BUZZER, 200 - (i * 8), 40);
    delay(50);
  }

  // --- FASE 5: DISIPACIÓN Y SILENCIO FINAL ---
  delay(400);
  noTone(PIN_BUZZER);
  tft.fillScreen(ILI9341_BLACK);
  delay(200);
}
// ============================================================================
// --- ANIMACIÓN DE HUIDA Y ACELERACIÓN DEL JUGADOR ---
// ============================================================================
void animacionHuidaJugador() {
  // Acelera la nave desde su posición actual hacia arriba hasta salir de pantalla (Y < -30)
  while (jugador.y > -30) {
    // Borrar posición previa
    tft.fillRect(jugador.x - 16, jugador.y - 18, 32, 36, COLOR_MAR);

    // Desplazar nave hacia arriba
    jugador.y -= 5;

    // Dibujar propulsor de fuego en la cola (Amarillo/Naranja)
    bool llamaLarga = ((millis() / 50) % 2 == 0);
    int largoFuego = llamaLarga ? 22 : 16;
    tft.fillTriangle(jugador.x, jugador.y + 10, jugador.x - 5, jugador.y + largoFuego, jugador.x + 5, jugador.y + largoFuego, ILI9341_YELLOW);
    tft.fillTriangle(jugador.x, jugador.y + 10, jugador.x - 2, jugador.y + (largoFuego - 4), jugador.x + 2, jugador.y + (largoFuego - 4), ILI9341_WHITE);

    // Dibujar avión del jugador 
    tft.fillRect(jugador.x - 14, jugador.y - 2, 28, 5, ILI9341_LIGHTGREY); 
    tft.fillTriangle(jugador.x, jugador.y - 16, jugador.x - 5, jugador.y + 10, jugador.x + 5, jugador.y + 10, ILI9341_WHITE); 
    tft.fillCircle(jugador.x, jugador.y - 3, 3, ILI9341_CYAN);
    // Tono de aceleración de turbinas
    tone(PIN_BUZZER, 400 + (300 - jugador.y * 2), 20);
    delay(15);
  }

  noTone(PIN_BUZZER);
  tft.fillScreen(COLOR_MAR);
  delay(300);
}
// ============================================================================
// --- INICIALIZACIÓN (SETUP) ---
// ============================================================================
void setup() {
  Serial.begin(115200);
  pinMode(PIN_PAUSA, INPUT_PULLUP);
  pinMode(PIN_BTN, INPUT_PULLUP);  // Habilita la resistencia Pull-Up interna para el Pin 13
  pinMode(JOY_BTN, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_MOTOR, OUTPUT);

  // Inicializar Pantalla TFT ILI9341
  tft.begin(40000000);
  tft.setRotation(0);  // Orientación Vertical (240x320)
  tft.fillScreen(ILI9341_BLACK);

  // Inicializar Almacenamiento MicroSD
  if (almacenamiento.begin()) {
    almacenamiento.cargarScores(topScores);
  }

  // Inicializar Conexión Wi-Fi y Cliente HiveMQ MQTT
  red.iniciarConexion();

  // Melodía de inicio
  reproducirMelodiaIntro();
}

// ============================================================================
// --- BUCLE PRINCIPAL (LOOP) ---
// ============================================================================
void loop() {
  // Mantener servicio MQTT en segundo plano
  red.actualizar();

  // --- LECTURA INDEPENDIENTE DE BOTONES ---

  // 1. Lectura del Botón del Joystick (Pin 32)
  bool joyLectura = (digitalRead(JOY_BTN) == LOW);
  bool btnJoyPulsado = (joyLectura && !botonJoyPrevio);
  botonJoyPrevio = joyLectura;

  // 2. Lectura del Botón de Acción (Pin 13)
  bool accionLectura = (digitalRead(PIN_BTN) == LOW);
  bool btnAccionPulsado = (accionLectura && !botonAccionPrevio);
  botonAccionPrevio = accionLectura;

  // 3. Lectura del Botón de Pausa (Pin 27)
  bool pausaPulsado = (digitalRead(PIN_PAUSA) == LOW && botonPausaPrevio == HIGH);
  botonPausaPrevio = digitalRead(PIN_PAUSA);

  switch (estado) {
    case INTRO:
      {
        if (!introDibujada) {
          tft.fillScreen(ILI9341_NAVY);
          tft.setTextColor(ILI9341_YELLOW);
          tft.setTextSize(3);
          tft.setCursor(20, 50);
          tft.print("MISSION 1945");
          tft.setTextColor(ILI9341_WHITE);
          tft.setTextSize(1);
          tft.setCursor(65, 120);
          tft.print("DESARROLLADO POR:");
          tft.setTextColor(ILI9341_CYAN);
          tft.setCursor(40, 150);
          tft.print("- Cristhian Padilla");
          tft.setCursor(40, 165);
          tft.print("- Octavio Moran");
          tft.setCursor(40, 180);
          tft.print("- Julian Gonzalez");
          tft.setCursor(40, 195);
          tft.print("- Andres Vazquez");

          introDibujada = true;  // Marca que ya se dibujó para no repetir fillScreen
        }
        bool visible = ((millis() / 300) % 2 == 0);
        tft.setCursor(45, 280);
        tft.setTextSize(1);
        if (visible) {  // Pinta texto blanco sobre fondo azul marino
          tft.setTextColor(ILI9341_WHITE, ILI9341_NAVY);
          tft.print("Presiona el Joystick...");
        } else {  // Pinta texto azul sobre fondo azul (lo oculta/borra limpiamente)
          tft.setTextColor(ILI9341_NAVY, ILI9341_NAVY);
          tft.print("Presiona el Joystick...");
        }
        if (btnJoyPulsado) {
          sonidoMenu();
          estado = MENU;
          introDibujada = false;  // Se resetea para si vuelve a la intro
          tft.fillScreen(ILI9341_BLACK);
        }
        break;
      }

    case MENU:
      {
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(2);
        tft.setCursor(60, 80);
        tft.print("MENU PRINCIPAL");

        // Navegación con Joystick Y
        int joyY = analogRead(JOY_Y);
        if (joyY > 3000 && menuSeleccion != 0) {
          menuSeleccion = 0;
          sonidoMenu();
          delay(150);
        }
        if (joyY < 1000 && menuSeleccion != 1) {
          menuSeleccion = 1;
          sonidoMenu();
          delay(150);
        }

        // Dibujar "Iniciar"
        tft.setCursor(60, 140);
        if (menuSeleccion == 0) {
          tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
          tft.print("> Iniciar");
        } else {
          tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
          tft.print("  Iniciar");
        }

        // Dibujar "Scores"
        tft.setCursor(60, 180);
        if (menuSeleccion == 1) {
          tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
          tft.print("> Scores ");
        } else {
          tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
          tft.print("  Scores ");
        }

        if (btnAccionPulsado) {  // Únicamente el Botón del Pin 13 confirma la opción seleccionada
          sonidoMenu();
          if (menuSeleccion == 0) {
            resetearJuego();
            estado = JUEGO;
            tft.fillScreen(COLOR_MAR);
          } else {
            estado = SCORES;
            tft.fillScreen(ILI9341_BLACK);
          }
        }
        break;
      }

    case SCORES:
      {
        tft.setTextColor(ILI9341_YELLOW);
        tft.setTextSize(2);
        tft.setCursor(70, 40);
        tft.print("TOP 5 SCORES");

        tft.setTextColor(ILI9341_WHITE);
        for (int i = 0; i < 5; i++) {
          tft.setCursor(90, 80 + (i * 25));
          tft.print(i + 1);
          tft.print(".");
          tft.print(topScores[i]);
        }

        tft.setTextSize(1);
        tft.setCursor(40, 280);
        tft.print("Presiona Joystick para volver");

        if (btnJoyPulsado || btnAccionPulsado) {
          sonidoMenu();
          estado = MENU;
          tft.fillScreen(ILI9341_BLACK);
        }
        break;
      }

    case JUEGO:
      {
        // 1. Audio de fondo en segundo plano
        if (millis() - ultimoTono > 500) {
          tone(PIN_BUZZER, notaAlta ? 200 : 150, 100);
          notaAlta = !notaAlta;
          ultimoTono = millis();
        }

        // 2. Control de Pausa
        if (pausaPulsado) {
          sonidoMenu();
          estado = PAUSA;
          tft.fillScreen(ILI9341_BLACK);
          break;
        }

        // 3. Movimiento del Jugador
        jugador.mover(analogRead(JOY_X), analogRead(JOY_Y));
        jugador.actualizarEfectos();

        // 4. Disparo del Jugador
        if (btnAccionPulsado && (millis() - ultimoDisparo > 400)) {
          ultimoDisparo = millis();
          if (jugador.dobleDisparoActivo) {
            balas.disparar(jugador.x - 6, jugador.y - 20);
            balas.disparar(jugador.x + 6, jugador.y - 20);
          } else {
            balas.disparar(jugador.x, jugador.y - 20);
          }
          sonidoDisparo();
        }

        // 4.1 Disparo automático de Naves Aliadas
        if (jugador.debeDispararAliados()) {
          balas.disparar(jugador.x - 22, jugador.y + 2);
          balas.disparar(jugador.x + 22, jugador.y + 2);
          sonidoDisparo();
        }

        // 5. Actualización de Entidades
        enemigos.actualizar(score);
        powerUp.actualizar(jugador.dobleDisparoActivo, jugador.escudoActivo);
        balas.actualizar();

        // 6. Colisiones: Balas vs Enemigo Común
        for (int i = 0; i < balas.getMaxBalas(); i++) {
          if (balas.isActiva(i)) {
            bool nivelCompletado = false;
            if (enemigos.verificarImpactoBala(balas.getX(i), balas.getY(i), score, nivelCompletado)) {
              balas.desactivar(i);
              sonidoImpacto();
              if (nivelCompletado) {
                if (nivel < 3) {
                  nivel++;
                  balas.reset();
                  powerUp.reset();
                  jugador.x = 120;
                  jugador.y = 280;
                  jugador.oldX = 120;
                  jugador.oldY = 280;
                  enemigos.configurarNivel(nivel);
                  estado = LEVEL_UP;
                  tft.fillScreen(COLOR_MAR);
                } else if (nivel == 3 && boss.getEstado() == BOSS_INACTIVO) {
                  // Al terminar el Nivel 3, entra el Boss Barco
                  boss.iniciarBatalla();
                }
              }
            }
          }
        }

        // --- 7. LÓGICA Y COLISIONES DEL BOSS (BLOQUE INDEPENDIENTE) ---
        if (boss.getEstado() != BOSS_INACTIVO) {
          // A) Actualizar posición y ataques del Boss
          boss.actualizar(jugador.x);

          // B) Colisión: Balas del jugador vs Puntos Débiles del Boss
          for (int i = 0; i < balas.getMaxBalas(); i++) {
            if (balas.isActiva(i)) {
              if (boss.verificarImpactoBala(balas.getX(i), balas.getY(i))) {
                balas.desactivar(i);
                sonidoImpacto();
                score += 200;
              }
            }
          }

          // C) Colisión: Proyectiles o Casco del Boss vs Jugador
          if (boss.verificarImpactoProyectilJugador(jugador.x, jugador.y) || boss.verificarColisionJugador(jugador.x, jugador.y)) {
            sonidoImpacto();
            jugador.recibirDano(tft);

            if (boss.verificarColisionJugador(jugador.x, jugador.y)) {
              jugador.y = constrain(jugador.y + 25, 30, 305);
            }

            if (jugador.vidas <= 0) {
              boss.reset();
              powerUp.reset();
              balas.reset();
              enemigos.reset();
              jugador.x = 120;
              jugador.y = 280;
              jugador.oldX = 120;
              jugador.oldY = 280;
              almacenamiento.guardarScore(score, topScores);
              imprimirResumenSerial("GAME OVER");
              red.enviarMetricasMQTT("GAME OVER", score, nivel, enemigos.enemigosDerrotados, jugador.vidas);
              estado = GAMEOVER;
              tft.fillScreen(ILI9341_BLACK);
              reproducirMelodiaGameOver();
              break;
            }
          }

          // D) Si el Boss es Derrotado -> Secuencia Final de Victoria
          if (boss.getEstado() == BOSS_DESTRUIDO) {
            boss.ejecutarAnimacionDestruccion(tft);
            animacionHuidaJugador();
            animacionBombaAtomica();

            score += (jugador.vidas * 1000) + 5000;
            almacenamiento.guardarScore(score, topScores);
            imprimirResumenSerial("VICTORIA");
            red.enviarMetricasMQTT("VICTORIA", score, nivel, enemigos.enemigosDerrotados, jugador.vidas);
            boss.reset();
            enemigos.reset();
            balas.reset();
            powerUp.reset();
            jugador.x = 120;
            jugador.y = 280;
            jugador.oldX = 120;
            jugador.oldY = 280;
            estado = VICTORIA;
            tft.fillScreen(ILI9341_BLACK);
            reproducirMelodiaVictoria();
            break;
          }
        }

        // 8. Colisiones: Enemigo vs Jugador
        if (enemigos.verificarColisionJugador(jugador.x, jugador.y)) {
          sonidoImpacto();
          bool teniaEscudo = jugador.escudoActivo;
          jugador.recibirDano(tft);

          if (!teniaEscudo) {
            digitalWrite(PIN_MOTOR, HIGH);
            tft.fillScreen(ILI9341_RED);
            delay(100);
            tft.fillScreen(COLOR_MAR);
            digitalWrite(PIN_MOTOR, LOW);

            if (jugador.vidas <= 0) {
              boss.reset();
              powerUp.reset();
              balas.reset();
              enemigos.reset();
              jugador.reset();
              almacenamiento.guardarScore(score, topScores);
              imprimirResumenSerial("GAME OVER");
              red.enviarMetricasMQTT("GAME OVER", score, nivel, enemigos.enemigosDerrotados, jugador.vidas);
              estado = GAMEOVER;
              tft.fillScreen(ILI9341_BLACK);
              reproducirMelodiaGameOver();
              break;
            }
          }
        }

        // 9. Colisiones: Jugador vs Power-Up
        TipoPowerUp obtenido = powerUp.verificarColision(jugador.x, jugador.y);
        if (obtenido == DOBLE_DISPARO) {
          jugador.activarDobleDisparo();
          sonidoMenu();
        } else if (obtenido == ESCUDO) {
          jugador.activarEscudo();
          sonidoMenu();
        } else if (obtenido == VIDA_EXTRA) {
          jugador.sumarVida();
          sonidoMenu();
        } else if (obtenido == ALIADOS) {
          jugador.activarAliados();
          sonidoMenu();
        }

        // 10. Dibujar HUD Superior
        tft.fillRect(0, 0, 240, 20, ILI9341_BLACK);
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(1);
        tft.setCursor(5, 5);
        tft.print("S:");
        tft.print(score);
        tft.setCursor(90, 5);
        tft.print("NIV:");
        tft.print(nivel);
        tft.setCursor(170, 5);
        tft.print("VIDAS:");
        tft.print(jugador.vidas);

        // 11. Renderizado en Pantalla TFT
        enemigos.dibujar(tft);
        balas.dibujar(tft);
        powerUp.dibujar(tft);
        jugador.dibujar(tft);
        if (boss.getEstado() != BOSS_INACTIVO) {
          boss.dibujar(tft);
        }

        delay(15);
        break;
      }

    case PAUSA:
      {
        tft.setTextColor(ILI9341_YELLOW);
        tft.setTextSize(3);
        tft.setCursor(70, 100);
        tft.print("PAUSA");

        tft.setTextSize(1);
        tft.setTextColor(ILI9341_WHITE);
        tft.setCursor(50, 160);
        tft.print("Presiona PAUSA para continuar");
        tft.setCursor(45, 180);
        tft.print("Presiona JOYSTICK para reiniciar");

        if (pausaPulsado) {
          sonidoMenu();
          tft.fillScreen(COLOR_MAR);
          estado = JUEGO;
        }
        if (btnJoyPulsado) {
          sonidoMenu();
          resetearJuego();
          tft.fillScreen(COLOR_MAR);
          estado = JUEGO;
        }
        break;
      }

    case GAMEOVER:
      {
        tft.setTextColor(ILI9341_RED);
        tft.setTextSize(3);
        tft.setCursor(45, 100);
        tft.print("GAME OVER");

        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(2);
        tft.setCursor(60, 150);
        tft.print("SCORE: ");
        tft.print(score);

        tft.setTextSize(1);
        tft.setCursor(50, 220);
        tft.print("Presiona Joystick al Menu");

        if (btnJoyPulsado) {
          sonidoMenu();
          estado = MENU;
          tft.fillScreen(ILI9341_BLACK);
        }
        break;
      }

    case LEVEL_UP:
      {
        tft.setTextColor(ILI9341_GREEN);
        tft.setTextSize(3);
        tft.setCursor(15, 90);
        tft.print("NIVEL ");
        tft.print(nivel - 1);
        tft.setCursor(15, 130);
        tft.print("COMPLETADO!");

        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(2);
        tft.setCursor(40, 180);
        tft.print("Siguiente: NIVEL ");
        tft.print(nivel);

        tft.setTextSize(1);
        tft.setCursor(35, 240);
        tft.print("Presiona Joystick para continuar");

        if (btnJoyPulsado) {
          sonidoMenu();
          tft.fillScreen(COLOR_MAR);
          estado = JUEGO;
        }
        break;
      }

    case VICTORIA:
      {
        powerUp.reset();
        balas.reset();
        enemigos.reset();
        jugador.reset();

        tft.setTextColor(ILI9341_YELLOW);
        tft.setTextSize(3);
        tft.setCursor(35, 90);
        tft.print("VICTORIA!");

        tft.setTextColor(ILI9341_CYAN);
        tft.setTextSize(2);
        tft.setCursor(30, 140);
        tft.print("Mision Cumplida");

        tft.setTextColor(ILI9341_WHITE);
        tft.setCursor(50, 180);
        tft.print("SCORE: ");
        tft.print(score);

        tft.setTextSize(1);
        tft.setCursor(40, 250);
        tft.print("Presiona Joystick para Menu");

        if (btnJoyPulsado) {
          sonidoMenu();
          estado = MENU;
          tft.fillScreen(ILI9341_BLACK);
        }
        break;
      }
  }
}


// ============================================================================
// --- IMPRESIÓN DE RESUMEN EN CONSOLA SERIAL ---
// ============================================================================
void imprimirResumenSerial(const char* resultado) {
  Serial.println("\n=================================");
  Serial.print(" RESUMEN DE PARTIDA: ");
  Serial.println(resultado);
  Serial.println("=================================");
  Serial.print("Puntaje Final:      ");
  Serial.println(score);
  Serial.print("Nivel Alcanzado:    ");
  Serial.println(nivel);
  Serial.print("Enemigos Derrotados:");
  Serial.println(enemigos.enemigosDerrotados);
  Serial.print("Vidas Restantes:    ");
  Serial.println(jugador.vidas);
  Serial.println("=================================\n");
}