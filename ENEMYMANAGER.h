#ifndef ENEMYMANAGER_H
#define ENEMYMANAGER_H

#include <Adafruit_ILI9341.h>

class EnemyManager {
private:
    float x, y;
    float oldX, oldY;
    bool activo;
    float velocidad;
    bool requiereBorrado; // Garantiza borrar el último cuadro al ser destruido

public:
    int enemigosDerrotados;
    int metaEnemigosNivel;

    EnemyManager();

    // Resetea el enemigo e inicia valores por defecto
    void reset();

    // Configura la velocidad base y la meta según el nivel alcanzado
    void configurarNivel(int nivel);

    // Actualiza el movimiento y la reaparición del enemigo
    void actualizar(int& score);

    // Dibuja el enemigo y borra rastros anteriores en la TFT
    void dibujar(Adafruit_ILI9341& tft);

    // Detección de colisiones exactas según "codigo juego.txt"
    bool verificarImpactoBala(int balaX, int balaY, int& score, bool& nivelCompletado);
    bool verificarColisionJugador(int playerX, int playerY);

    // Métodos de lectura (Getters)
    float getX() const { return x; }
    float getY() const { return y; }
    bool isActivo() const { return activo; }
    float getVelocidad() const { return velocidad; }
};

#endif