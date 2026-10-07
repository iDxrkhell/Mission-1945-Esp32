#include "Player.h"
#include "Config.h"

Player::Player() {
  reset();
}

void Player::reset() {
  x = 120;
  y = 280;
  oldX = 120;
  oldY = 280;
  vidas = 3;
  escudoActivo = false;
  dobleDisparoActivo = false;
  tiempoEscudo = 0;
  tiempoDobleDisparo = 0;
  escudoDibujadoPrevio = false;
  aliadosActivos = false;
  tiempoAliados = 0;
  ultimoDisparoAliados = 0;
  aliadosDibujadosPrevios = false;
}

void Player::mover(int joyX, int joyY) {
  oldX = x;
  oldY = y;

  // Lectura de ejes del Joystick (idéntico al código original)
  if (joyX > 3000) x -= 3;
  if (joyX < 1000) x += 3;
  if (joyY > 3000) y -= 3;
  if (joyY < 1000) y += 3;

  // Delimitar dentro del área jugable (240x320)
  x = constrain(x, 15, 225);
  y = constrain(y, 30, 305);
}

void Player::actualizarEfectos() {
  if (dobleDisparoActivo && (millis() - tiempoDobleDisparo > 8000)) {
    dobleDisparoActivo = false;
  }
  if (escudoActivo && (millis() - tiempoEscudo > 10000)) {
    escudoActivo = false;
  }
  if (aliadosActivos && (millis() - tiempoAliados > 10000)) {  // Duran 10 segundos
    aliadosActivos = false;
  }
}

void Player::activarAliados() {
  aliadosActivos = true;
  tiempoAliados = millis();
}

bool Player::debeDispararAliados() {
  if (aliadosActivos && (millis() - ultimoDisparoAliados > 500)) {  // Disparan cada 500ms
    ultimoDisparoAliados = millis();
    return true;
  }
  return false;
}

void Player::activarEscudo() {
  escudoActivo = true;
  tiempoEscudo = millis();
}

void Player::activarDobleDisparo() {
  dobleDisparoActivo = true;
  tiempoDobleDisparo = millis();
}

void Player::sumarVida() {
  if (vidas < 5) {
    vidas++;
  }
}

void Player::recibirDano(Adafruit_ILI9341& tft) {
  if (escudoActivo) {
    escudoActivo = false;  // El escudo absorbe el impacto y se desactiva
  } else {
    if (vidas > 0) {
      vidas--;
    }
  }
}

void Player::dibujar(Adafruit_ILI9341& tft) {
  // Color de las naves aliadas (Azul Claro / Cyan Cían)
  uint16_t colorAliado = 0x7FFF;
  uint16_t colorFondo = COLOR_MAR;

  // 1. Borrar rastro de la nave principal y de los aliados si se movió
  if (oldX != x || oldY != y) {
    tft.fillRect(oldX - 16, oldY - 18, 32, 36, colorFondo);
    if (escudoDibujadoPrevio) {
      tft.drawCircle(oldX, oldY - 2, 18, COLOR_MAR);
    }
    if (aliadosDibujadosPrevios) {
      // Borrar aliados en su posición previa
      tft.fillTriangle(oldX - 22, oldY + 2, oldX - 28, oldY + 14, oldX - 16, oldY + 14, COLOR_MAR);
      tft.fillTriangle(oldX + 22, oldY + 2, oldX + 28, oldY + 14, oldX + 16, oldY + 14, COLOR_MAR);
    }
  } else {
    // Si no se movió pero el escudo o los aliados acaban de desactivarse, borra los rastros
    if (escudoDibujadoPrevio && !escudoActivo) {
      tft.drawCircle(x, y - 2, 18, COLOR_MAR);
      escudoDibujadoPrevio = false;
    }
    if (aliadosDibujadosPrevios && !aliadosActivos) {
      tft.fillTriangle(x - 22, y + 2, x - 28, y + 14, x - 16, y + 14, COLOR_MAR);
      tft.fillTriangle(x + 22, y + 2, x + 28, y + 14, x + 16, y + 14, COLOR_MAR);
      aliadosDibujadosPrevios = false;
    }
  }

  // 2\. DIBUJAR AVIONCITO DEL JUGADOR (Apuntando hacia arriba) 
  // Alas principales (Gris Claro) 
  tft.fillRect(x - 14, y - 2, 28, 5, ILI9341_LIGHTGREY); 
  // Fuselaje principal (Triángulo Blanco) 
  tft.fillTriangle(x, y - 16, x - 5, y + 10, x + 5, y + 10, ILI9341_WHITE); 
  // Cabina de cristal (Círculo Cyan) 
  tft.fillCircle(x, y - 3, 3, ILI9341_CYAN); 
  // Alerón trasero (Gris Oscuro) 
  tft.fillRect(x - 8, y + 8, 16, 3, ILI9341_DARKGREY);

  // 3. Dibujar ESCUDO con PARPADEO en los últimos 2 segundos
  if (escudoActivo) {
    unsigned long tiempoTranscurrido = millis() - tiempoEscudo;
    bool visible = true;
    if (tiempoTranscurrido > 8000) {          // Últimos 2 segundos (de 8s a 10s)
      visible = ((millis() / 150) % 2 == 0);  // Parpadeo alternando cada 150ms
    }
    if (visible) {
      tft.drawCircle(x, y - 2, 18, ILI9341_CYAN);
      escudoDibujadoPrevio = true;
    } else {
      tft.drawCircle(x, y - 2, 18, ILI9341_BLACK);
    }
  }

  // 4. Dibujar ALIADOS con PARPADEO en los últimos 2 segundos
  if (aliadosActivos) {
    unsigned long tiempoTranscurrido = millis() - tiempoAliados;
    bool visible = true;
    if (tiempoTranscurrido > 8000) {          // Últimos 2 segundos
      visible = ((millis() / 150) % 2 == 0);  // Parpadeo
    }
    if (visible) {
      // Aliado Izquierdo (más pequeño, triángulo de 12x12 px)
      tft.fillTriangle(x - 22, y + 2, x - 28, y + 14, x - 16, y + 14, colorAliado);
      // Aliado Derecho
      tft.fillTriangle(x + 22, y + 2, x + 28, y + 14, x + 16, y + 14, colorAliado);
      aliadosDibujadosPrevios = true;
    } else {
      // Borrar temporalmente en el fotograma apagado del parpadeo
      tft.fillTriangle(x - 22, y + 2, x - 28, y + 14, x - 16, y + 14, COLOR_MAR);
      tft.fillTriangle(x + 22, y + 2, x + 28, y + 14, x + 16, y + 14, COLOR_MAR);
    }
  }
}