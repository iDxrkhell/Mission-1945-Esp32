#include "Boss.h"
#include "Config.h"

Boss::Boss() {
  reset();
}

void Boss::reset() {
  x = 0;
  y = -70;
  ancho = 240;
  alto = 60;
  estado = BOSS_INACTIVO;
  fase = 1;
  tiempoEstado = 0;
  ultimoDisparo = 0;
  ultimoMisil = 0;

  // Inicializar piscina de proyectiles enemigos
  for (int i = 0; i < MAX_PROYECTILES; i++) {
    proyectiles[i].x = -20;
    proyectiles[i].y = -20;
    proyectiles[i].oldX = -20;
    proyectiles[i].oldY = -20;
    proyectiles[i].vx = 0;
    proyectiles[i].vy = 0;
    proyectiles[i].activo = false;
    proyectiles[i].esMisil = false;
    proyectiles[i].requiereBorrado = false;
  }
  configurarPuntosDebiles();
}

void Boss::iniciarBatalla() {
  reset();
  estado = BOSS_ENTRANDO;
  y = -70;
  fase = 1;
  configurarPuntosDebiles();
}

void Boss::configurarPuntosDebiles() {
  if (fase == 1) {
    puntosDebiles[0] = { 40, 45, false, false };
    puntosDebiles[1] = { 120, 35, false, false };
    puntosDebiles[2] = { 200, 45, false, false };
  } else if (fase == 2) {
    puntosDebiles[0] = { 60, 25, false, false };
    puntosDebiles[1] = { 120, 55, false, false };
    puntosDebiles[2] = { 180, 25, false, false };
  } else if (fase == 3) {
    puntosDebiles[0] = { 30, 55, false, false };
    puntosDebiles[1] = { 120, 65, false, false };
    puntosDebiles[2] = { 210, 55, false, false };
  }
}

void Boss::dispararBalas() {
  // Orígenes de las 3 torretas en el buque
  int torretasX[3] = { 40, 120, 200 };
  for (int t = 0; t < 3; t++) {
    for (int i = 0; i < MAX_PROYECTILES; i++) {
      if (!proyectiles[i].activo) {
        proyectiles[i].x = torretasX[t];
        proyectiles[i].y = y + 45;
        proyectiles[i].oldX = proyectiles[i].x;
        proyectiles[i].oldY = proyectiles[i].y;
        proyectiles[i].vx = 0;
        proyectiles[i].vy = 3.0 + (fase * 0.5);  // Incrementa velocidad en cada fase
        proyectiles[i].ancho = 4;
        proyectiles[i].alto = 8;
        proyectiles[i].esMisil = false;
        proyectiles[i].activo = true;
        proyectiles[i].requiereBorrado = false;
        break;
      }
    }
  }
}

void Boss::dispararMisil(float objetivoX) {
  for (int i = 0; i < MAX_PROYECTILES; i++) {
    if (!proyectiles[i].activo) {
      proyectiles[i].x = (i % 2 == 0) ? 30 : 210;  // Lanza alternado desde costados
      proyectiles[i].y = y + 30;
      proyectiles[i].oldX = proyectiles[i].x;
      proyectiles[i].oldY = proyectiles[i].y;

      // Calcular trayectoria hacia la posición del jugador
      float dx = objetivoX - proyectiles[i].x;
      proyectiles[i].vx = (dx > 0) ? 1.5 : -1.5;
      proyectiles[i].vy = 4.0;  // Velocidad mayor
      proyectiles[i].ancho = 6;
      proyectiles[i].alto = 12;
      proyectiles[i].esMisil = true;
      proyectiles[i].activo = true;
      proyectiles[i].requiereBorrado = false;
      break;
    }
  }
}

void Boss::actualizarProyectiles() {
  for (int i = 0; i < MAX_PROYECTILES; i++) {
    if (proyectiles[i].activo) {
      proyectiles[i].oldX = proyectiles[i].x;
      proyectiles[i].oldY = proyectiles[i].y;
      proyectiles[i].x += proyectiles[i].vx;
      proyectiles[i].y += proyectiles[i].vy;

      // Desactivar si sale de los bordes de la pantalla (240x320)
      if (proyectiles[i].y > 320 || proyectiles[i].x < 0 || proyectiles[i].x > 240) {
        proyectiles[i].activo = false;
        proyectiles[i].requiereBorrado = true;
      }
    }
  }
}

void Boss::actualizar(int playerX) {
  if (estado == BOSS_INACTIVO) return;

  // 1. Animación de entrada
  if (estado == BOSS_ENTRANDO) {
    y += 1;
    if (y >= 20) {
      y = 20;
      estado = BOSS_ATACANDO;
      tiempoEstado = millis();
    }
    return;
  }

  // 2. MODO ATACANDO: Generar ráfagas según la Fase
  if (estado == BOSS_ATACANDO) {
    unsigned long cadenciaBalas = 1300 - (fase * 150);  // 950ms, 700ms, 450ms
    if (millis() - ultimoDisparo > cadenciaBalas) {
      ultimoDisparo = millis();
      dispararBalas();
    }

    if (fase >= 2) {
      unsigned long cadenciaMisil = (fase == 2) ? 2500 : 1800;
      if (millis() - ultimoMisil > cadenciaMisil) {
        ultimoMisil = millis();
        dispararMisil(playerX);
      }
    }

    // Transición a MODO_SOBRECALENTADO
    unsigned long duracionAtaque = 9000 - (fase * 1000);
    if (millis() - tiempoEstado > duracionAtaque) {
      estado = BOSS_SOBRECALENTADO;
      tiempoEstado = millis();
    }
  }

  // 3. MODO SOBRECALENTADO: Permanece vulnerable 5 segundos
  if (estado == BOSS_SOBRECALENTADO) {
    if (millis() - tiempoEstado > 5000) {
      estado = BOSS_ATACANDO;
      tiempoEstado = millis();
    }
  }

  // Actualizar posición de proyectiles lanzados
  actualizarProyectiles();
}

void Boss::dibujar(Adafruit_ILI9341& tft) {
  if (estado == BOSS_INACTIVO) return;

  // --- 1. DIBUJAR Y BORRAR RASTROS DE PROYECTILES ENEMIGOS ---
  for (int i = 0; i < MAX_PROYECTILES; i++) {
    if (proyectiles[i].requiereBorrado || (proyectiles[i].activo && (proyectiles[i].oldX != proyectiles[i].x || proyectiles[i].oldY != proyectiles[i].y))) {
      tft.fillRect(proyectiles[i].oldX - (proyectiles[i].ancho / 2), proyectiles[i].oldY - (proyectiles[i].alto / 2), proyectiles[i].ancho, proyectiles[i].alto, COLOR_MAR);
      proyectiles[i].requiereBorrado = false;
    }

    if (proyectiles[i].activo) {
      uint16_t color = proyectiles[i].esMisil ? ILI9341_ORANGE : ILI9341_RED;
      tft.fillRect(proyectiles[i].x - (proyectiles[i].ancho / 2), proyectiles[i].y - (proyectiles[i].alto / 2), proyectiles[i].ancho, proyectiles[i].alto, color);
    }
  }

  // --- 2. DIBUJO DEL CASCO Y ESTRUCTURA DEL BARCO ---
  tft.fillRect(0, y, 240, 40, ILI9341_DARKGREY);
  tft.drawRect(0, y, 240, 40, ILI9341_WHITE);
  tft.fillRect(30, y + 40, 180, 20, ILI9341_NAVY);
  tft.drawRect(30, y + 40, 180, 20, ILI9341_WHITE);
  tft.fillRect(35, y + 10, 15, 25, ILI9341_LIGHTGREY);
  tft.fillRect(110, y + 5, 20, 25, ILI9341_LIGHTGREY);
  tft.fillRect(190, y + 10, 15, 25, ILI9341_LIGHTGREY);

  // --- 3. EFECTOS EN PUNTOS DÉBILES ---
  for (int i = 0; i < 3; i++) {
    int px = puntosDebiles[i].x;
    int py = y + (puntosDebiles[i].y - 20);

    if (puntosDebiles[i].destruido) {
      tft.fillCircle(px, py, 8, ILI9341_BLACK);
      tft.drawCircle(px, py, 8, ILI9341_RED);
    } else if (estado == BOSS_SOBRECALENTADO) {
      bool parpadeo = ((millis() / 120) % 2 == 0);
      uint16_t colorLuz = parpadeo ? ILI9341_YELLOW : ILI9341_RED;
      tft.fillCircle(px, py, 9, colorLuz);
      tft.drawCircle(px, py, 11, ILI9341_WHITE);
    } else {
      tft.fillCircle(px, py, 7, ILI9341_MAROON);
      tft.drawCircle(px, py, 7, ILI9341_DARKGREY);
    }
  }
}

bool Boss::verificarImpactoBala(int balaX, int balaY) {
  if (estado != BOSS_SOBRECALENTADO) return false;

  for (int i = 0; i < 3; i++) {
    if (!puntosDebiles[i].destruido) {
      int px = puntosDebiles[i].x;
      int py = y + (puntosDebiles[i].y - 20);

      if (abs(balaX - px) < 12 && abs(balaY - py) < 12) {
        puntosDebiles[i].destruido = true;
        bool faseCompletada = true;

        for (int j = 0; j < 3; j++) {
          if (!puntosDebiles[j].destruido) {
            faseCompletada = false;
            break;
          }
        }

        if (faseCompletada) {
          if (fase < 3) {
            fase++;
            configurarPuntosDebiles();
            estado = BOSS_ATACANDO;
            tiempoEstado = millis();
          } else {
            estado = BOSS_DESTRUIDO;
          }
        }
        return true;
      }
    }
  }
  return false;
}

bool Boss::verificarColisionJugador(int playerX, int playerY) {
  if (estado == BOSS_INACTIVO || estado == BOSS_DESTRUIDO) return false;
  if (playerY <= (y + alto + 10)) return true;
  return false;
}

bool Boss::verificarImpactoProyectilJugador(int playerX, int playerY) {
  for (int i = 0; i < MAX_PROYECTILES; i++) {
    if (proyectiles[i].activo) {
      if (abs(playerX - proyectiles[i].x) < 14 && abs(playerY - proyectiles[i].y) < 14) {
        proyectiles[i].activo = false;
        proyectiles[i].requiereBorrado = true;
        return true;  // Proyectil enemigo impactó al jugador
      }
    }
  }
  return false;
}

void Boss::ejecutarAnimacionDestruccion(Adafruit_ILI9341& tft) {
  // 1. Desactivar y limpiar proyectiles enemigos restantes
  for (int i = 0; i < MAX_PROYECTILES; i++) {
    if (proyectiles[i].activo) {
      tft.fillRect(proyectiles[i].x - (proyectiles[i].ancho / 2), proyectiles[i].y - (proyectiles[i].alto / 2), proyectiles[i].ancho, proyectiles[i].alto, COLOR_MAR);
      proyectiles[i].activo = false;
    }
  }

  // 2. Ráfaga de explosiones múltiples sobre el barco (30 detonaciones)
  digitalWrite(PIN_MOTOR, HIGH);
  for (int e = 0; e < 30; e++) {
    int expX = random(10, 230);
    int expY = y + random(0, alto);
    int radio = random(10, 25);

    // Renderizado del fogonazo (Blanco -> Amarillo -> Naranja)
    tft.fillCircle(expX, expY, radio, ILI9341_WHITE);
    tone(PIN_BUZZER, random(150, 600), 40);
    delay(30);

    tft.fillCircle(expX, expY, radio, ILI9341_YELLOW);
    delay(20);

    tft.fillCircle(expX, expY, radio + 2, ILI9341_ORANGE);
    delay(20);

    // Dejar marca de quemado en negro
    tft.fillCircle(expX, expY, radio + 2, COLOR_MAR);
  }
  digitalWrite(PIN_MOTOR, LOW);

  // 3. Borrar completamente el casco restante
  tft.fillRect(0, y, 240, alto + 10, COLOR_MAR);
  estado = BOSS_INACTIVO;
}