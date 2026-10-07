#include "PowerUp.h"
#include "config.h"

PowerUp::PowerUp() {
  reset();
}

void PowerUp::reset() {
  for (int i = 0; i < 4; i++) {
    items[i].x = -20;
    items[i].y = -20;
    items[i].activo = false;
    items[i].requiereBorrado = false;
  }
  items[0].tipo = DOBLE_DISPARO;
  items[1].tipo = ESCUDO;
  items[2].tipo = VIDA_EXTRA;
  items[3].tipo = ALIADOS;
}

void PowerUp::actualizar(bool dobleDisparoJugador, bool escudoJugador) {
  int activos = 0;
  for (int i = 0; i < 4; i++) {
    if (items[i].activo) activos++;
  }
  // 1. DISPARO DOBLE ('D' - Cyan)
  if (activos < 2 && !items[0].activo && !dobleDisparoJugador && random(0, 450) == 50) {
    items[0].x = random(30, 210);
    items[0].y = random(30, 280);
    items[0].activo = true;
    items[0].tiempoSpawn = millis();
    items[0].requiereBorrado = false;
    activos++;
  }
  if (items[0].activo && (millis() - items[0].tiempoSpawn > 6000)) {
    items[0].activo = false;
    items[0].requiereBorrado = true;
  }

  // 2. ESCUDO PROTECTOR ('E' - Azul)
  if (activos < 2 && !items[1].activo && !escudoJugador && random(0, 450) == 75) {
    items[1].x = random(30, 210);
    items[1].y = random(30, 280);
    items[1].activo = true;
    items[1].tiempoSpawn = millis();
    items[1].requiereBorrado = false;
    activos++;
  }
  if (items[1].activo && (millis() - items[1].tiempoSpawn > 6000)) {
    items[1].activo = false;
    items[1].requiereBorrado = true;
  }

  // 3. VIDA EXTRA ('V' - Verde)
  if (activos < 2 && !items[2].activo && random(0, 700) == 120) {
    items[2].x = random(30, 210);
    items[2].y = random(30, 280);
    items[2].activo = true;
    items[2].tiempoSpawn = millis();
    items[2].requiereBorrado = false;
    activos++;
  }
  if (items[2].activo && (millis() - items[2].tiempoSpawn > 6000)) {
    items[2].activo = false;
    items[2].requiereBorrado = true;
  }

  // 4. COMPAÑEROS / ALIADOS ('A' - Magenta)
  if (activos < 2 && !items[3].activo && random(0, 700) == 90) {
    items[3].x = random(30, 210);
    items[3].y = random(30, 280);
    items[3].activo = true;
    items[3].tiempoSpawn = millis();
    items[3].requiereBorrado = false;
  }

  if (items[3].activo && (millis() - items[3].tiempoSpawn > 6000)) {
    items[3].activo = false;
    items[3].requiereBorrado = true;
  }
}

void PowerUp::dibujar(Adafruit_ILI9341& tft) {
  for (int i = 0; i < 4; i++) {
    // 1. Borrar recuadro negro si el ítem expiró por completo o fue recogido
    if (items[i].requiereBorrado) {
      tft.fillRect(items[i].x - 6, items[i].y - 6, 12, 12, COLOR_MAR);
      items[i].requiereBorrado = false;
    }

    // 2. Dibujar ítem activo en pantalla con efecto de parpadeo
    if (items[i].activo) {
      unsigned long tiempoEnPantalla = millis() - items[i].tiempoSpawn;
      bool visible = true;

      // Si lleva más de 3 segundos en pantalla (faltan 3s para expirar), inicia el parpadeo
      if (tiempoEnPantalla > 3000) {
        visible = ((millis() / 150) % 2 == 0);  // Alterna visibilidad cada 150ms
      }

      if (visible) {
        uint16_t colorFondo = ILI9341_CYAN;
        uint16_t colorTexto = ILI9341_BLACK;
        const char* letra = "D";

        if (items[i].tipo == ESCUDO) {
          colorFondo = ILI9341_BLUE;
          colorTexto = ILI9341_WHITE;
          letra = "E";
        } else if (items[i].tipo == VIDA_EXTRA) {
          colorFondo = ILI9341_GREEN;
          colorTexto = ILI9341_WHITE;
          letra = "V";
        } else if (items[i].tipo == ALIADOS) {
          colorFondo = ILI9341_MAGENTA;
          colorTexto = ILI9341_WHITE;
          letra = "A";
        }

        // Dibujar cuadro del Power-Up con su letra
        tft.fillRect(items[i].x - 6, items[i].y - 6, 12, 12, colorFondo);
        tft.setTextColor(colorTexto);
        tft.setTextSize(1);
        tft.setCursor(items[i].x - 3, items[i].y - 4);
        tft.print(letra);
      } else {
        // En los cuadros intermedios del parpadeo, borra el recuadro temporalmente a negro
        tft.fillRect(items[i].x - 6, items[i].y - 6, 12, 12, COLOR_MAR);
      }
    }
  }
}

TipoPowerUp PowerUp::verificarColision(int playerX, int playerY) {
  for (int i = 0; i < 4; i++) {
    if (items[i].activo) {
      // Tolerancia exacta del original: abs(jugX - powX) < 18 && abs(jugY - powY) < 18
      if (abs(playerX - items[i].x) < 18 && abs(playerY - items[i].y) < 18) {
        items[i].activo = false;
        items[i].requiereBorrado = true;
        return items[i].tipo;
      }
    }
  }
  return NINGUNO;
}

void PowerUp::desactivar() {
  for (int i = 0; i < 4; i++) {
    if (items[i].activo) {
      items[i].activo = false;
      items[i].requiereBorrado = true;
    }
  }
}