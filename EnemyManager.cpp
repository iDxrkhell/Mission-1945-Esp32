#include "EnemyManager.h"
#include "config.h"
EnemyManager::EnemyManager() {
    reset();
}

void EnemyManager::reset() {
    x = 120;
    y = -20;
    oldX = 120;
    oldY = -20;
    activo = false;
    velocidad = 2.0;
    enemigosDerrotados = 0;
    metaEnemigosNivel = 10;
    requiereBorrado = false;
}

void EnemyManager::configurarNivel(int nivel) {
    enemigosDerrotados = 0;
    activo = false;
    requiereBorrado = true;

    if (nivel == 1) {
        metaEnemigosNivel = 10;
        velocidad = 2.0;
    } else if (nivel == 2) {
        metaEnemigosNivel = 20;
        velocidad = 2.0 + 0.5;
    } else {
        metaEnemigosNivel = 25;
        velocidad = 2.0 + 1.0;
    }
}

void EnemyManager::actualizar(int& score) {
    if (!activo) {
        // Generar nuevo enemigo en posición X aleatoria
        x = random(20, 220);
        y = -20;
        activo = true;
    } else {
        oldX = x;
        oldY = y;
        y += velocidad;

        // Si el enemigo escapa por la parte inferior de la pantalla (Y > 320)
        if (y > 320) {
            activo = false;
            requiereBorrado = true;
            // Penalización idéntica al código original (-100 puntos, mínimo 0)
            score = max(0, score - 100);
        }
    }
}

void EnemyManager::dibujar(Adafruit_ILI9341& tft) {
    // 1. Borrar rastro si el enemigo se movió o se desactivó recientemente
    if (requiereBorrado || (oldX != x || oldY != y)) {
        tft.fillRect(oldX - 14, oldY - 14, 28, 28, COLOR_MAR);
        requiereBorrado = false;
    }

    // 2. Si no está activo pero no hubo rastro borrado, limpiar en oldX/oldY
    if (!activo) {
        tft.fillRect(oldX - 10, oldY - 10, 20, 20, COLOR_MAR);
    } else {
        // 3. DIBUJAR AVIONCITO ENEMIGO (Apuntando hacia abajo) 
        // Alas principales (Verde Oscuro) 
        tft.fillRect(x - 12, y - 4, 24, 4, ILI9341_DARKGREEN); 
        // Fuselaje principal (Triángulo Rojo invertido) 
        tft.fillTriangle(x, y + 12, x - 5, y - 10, x + 5, y - 10, ILI9341_RED); 
        // Cabina amarilla 
        tft.fillCircle(x, y - 2, 2, ILI9341_YELLOW);
    }
}

bool EnemyManager::verificarImpactoBala(int balaX, int balaY, int& score, bool& nivelCompletado) {
    nivelCompletado = false;
    if (activo) {
        // Tolerancia exacta del original: distancia absoluta menor a 15 píxeles en X e Y
        if (abs(balaX - x) < 15 && abs(balaY - y) < 15) {
            activo = false;
            requiereBorrado = true;
            score += 100;
            enemigosDerrotados++;
            velocidad += 0.1; // Aumenta la velocidad progresivamente con cada baja

            if (enemigosDerrotados >= metaEnemigosNivel) {
                nivelCompletado = true;
            }
            return true; // Hubo impacto
        }
    }
    return false;
}

bool EnemyManager::verificarColisionJugador(int playerX, int playerY) {
    if (activo) {
        // Tolerancia exacta del original: distancia absoluta menor a 20 píxeles en X e Y
        if (abs(playerX - x) < 20 && abs(playerY - y) < 20) {
            activo = false;
            requiereBorrado = true;
            return true; // Colisionó con el jugador
        }
    }
    return false;
}