#include "Bullet.h"
#include "config.h"
BulletManager::BulletManager() {
    reset();
}

void BulletManager::reset() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        balas[i].x = -10;
        balas[i].y = -10;
        balas[i].oldX = -10;
        balas[i].oldY = -10;
        balas[i].activa = false;
        balas[i].requiereBorrado = false;
    }
}

void BulletManager::disparar(int x, int y) {
    // Buscar una ranura libre en la piscina de balas
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!balas[i].activa) {
            balas[i].x = x;
            balas[i].y = y;
            balas[i].oldX = x;
            balas[i].oldY = y;
            balas[i].activa = true;
            balas[i].requiereBorrado = false;
            break; // Disparada con éxito
        }
    }
}

void BulletManager::actualizar() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (balas[i].activa) {
            balas[i].oldX = balas[i].x;
            balas[i].oldY = balas[i].y;
            balas[i].y -= 6; // Velocidad exacta del código original

            // Si la bala sale por la parte superior de la pantalla
            if (balas[i].y < 0) {
                desactivar(i);
            }
        }
    }
}

void BulletManager::desactivar(int index) {
    if (index >= 0 && index < MAX_BULLETS) {
        if (balas[index].activa) {
            balas[index].activa = false;
            balas[index].requiereBorrado = true;
        }
    }
}

void BulletManager::dibujar(Adafruit_ILI9341& tft) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        // 1. Borrar rastro anterior si la bala se movió o acaba de desactivarse
        if (balas[i].requiereBorrado || (balas[i].oldX != balas[i].x || balas[i].oldY != balas[i].y)) {
            tft.fillCircle(balas[i].oldX, balas[i].oldY, 2, COLOR_MAR);
            balas[i].requiereBorrado = false;
        }

        // 2. Dibujar la bala activa (Círculo Amarillo de radio 2)
        if (balas[i].activa) {
            tft.fillCircle(balas[i].x, balas[i].y, 2, ILI9341_YELLOW);
        }
    }
}