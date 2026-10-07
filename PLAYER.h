#ifndef PLAYER_H
#define PLAYER_H

#include <Adafruit_ILI9341.h>

class Player {
private:
    bool escudoDibujadoPrevio; // Guarda si en el frame anterior habia un halo dibujado
    bool aliadosDibujadosPrevios; // Control de borrado de rastro de aliados

public:
    int x, y;
    int oldX, oldY;
    int vidas;

    // Estados de Power-Ups
    bool escudoActivo;
    bool dobleDisparoActivo;
    bool aliadosActivos;

    unsigned long tiempoEscudo;
    unsigned long tiempoDobleDisparo;
    unsigned long tiempoAliados; // <--- NUEVO
    unsigned long ultimoDisparoAliados; // <--- NUEVO

    Player();

    // Restablece los valores iniciales para una nueva partida
    void reset();

    // Lee la entrada analógica del Joystick y restringe la posición
    void mover(int joyX, int joyY);

    // Controla la expiración por tiempo de los power-ups
    void actualizarEfectos();

    // Métodos para activar modificadores de forma limpia
    void activarEscudo();
    void activarDobleDisparo();

    void activarAliados(); // <--- NUEVO
    bool debeDispararAliados(); // <--- NUEVO
    void sumarVida();

    // Procesa el daño recibido (absorbido por escudo o resta vida)
    void recibirDano(Adafruit_ILI9341& tft);

    // Dibuja la nave y el escudo gestionando el borrado de rastros sin parpadeo
    void dibujar(Adafruit_ILI9341& tft);
};

#endif
