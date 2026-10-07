#ifndef POWERUP_H
#define POWERUP_H

#include <Adafruit_ILI9341.h>

// Tipos de mejoras disponibles en Mission 1945
enum TipoPowerUp { NINGUNO, DOBLE_DISPARO, ESCUDO, VIDA_EXTRA, ALIADOS };

// Estructura interna para gestionar cada ítem flotando en pantalla
struct PowerUpItem {
    int x, y;
    bool activo;
    TipoPowerUp tipo;
    unsigned long tiempoSpawn;
    bool requiereBorrado;
};

class PowerUp {
private:
    PowerUpItem items[4]; // Un espacio por cada tipo de power-up (Doble Disparo, Escudo, Vida)

public:
    PowerUp();

    // Reinicia y desactiva todos los ítems
    void reset();

    // Genera aleatoriamente e ítemiza la expiración tras 6 segundos
    void actualizar(bool dobleDisparoJugador, bool escudoJugador);

    // Dibuja los cuadros (Cyan, Azul, Verde) con sus letras (D, E, V) y borra los expirados
    void dibujar(Adafruit_ILI9341& tft);

    // Comprueba colisión con la nave del jugador y retorna el tipo recolectado
    TipoPowerUp verificarColision(int playerX, int playerY);

    // Desactiva todos los ítems en pantalla
    void desactivar();
};

#endif