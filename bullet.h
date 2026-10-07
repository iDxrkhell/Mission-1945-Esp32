#ifndef BULLET_H
#define BULLET_H

#include <Adafruit_ILI9341.h>

// Estructura para gestionar cada proyectil individualmente
struct BulletItem {
    int x, y;
    int oldX, oldY;
    bool activa;
    bool requiereBorrado; // Garantiza borrar el rastro cuando la bala desaparece
};

class BulletManager {
private:
    static const int MAX_BULLETS = 8;
    BulletItem balas[MAX_BULLETS];

public:
    BulletManager();

    // Reinicia todas las balas
    void reset();

    // Dispara una nueva bala desde las coordenadas (x, y)
    void disparar(int x, int y);

    // Actualiza la posición Y de todas las balas activas (-6 px/frame)
    void actualizar();

    // Dibuja las balas amarillas y limpia el rastro negro anterior
    void dibujar(Adafruit_ILI9341& tft);

    // Desactiva una bala específica (por colisión o salida de pantalla)
    void desactivar(int index);

    // Métodos de acceso (Getters) para la comprobación de colisiones
    int getMaxBalas() const { return MAX_BULLETS; }
    bool isActiva(int index) const { return balas[index].activa; }
    int getX(int index) const { return balas[index].x; }
    int getY(int index) const { return balas[index].y; }
};

#endif