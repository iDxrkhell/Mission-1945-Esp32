#ifndef BOSS_H
#define BOSS_H

#include <Adafruit_ILI9341.h>

// Estados internos de la batalla contra el Boss
enum EstadoBoss {
    BOSS_INACTIVO,
    BOSS_ENTRANDO,       // Animación inicial donde el barco baja a posición
    BOSS_ATACANDO,       // Invulnerable: esquivar proyectiles
    BOSS_SOBRECALENTADO, // Vulnerable: se iluminan los 3 puntos débiles
    BOSS_DESTRUIDO       // Ejecutando explosiones finales
};

struct PuntoDebil {
    int x, y;
    bool destruido;
    bool requiereBorrado;
};

// Estructura para gestionar los proyectiles lanzados por el Boss
struct BossProyectil {
    float x, y;
    float vx, vy;
    int ancho, alto;
    bool activo;
    bool esMisil; // true = Misil grande (Naranja), false = Bala estándar (Roja)
    float oldX, oldY;
    bool requiereBorrado;
};

class Boss {
private:
    int x, y;
    int ancho, alto;
    EstadoBoss estado;
    int fase;
    unsigned long tiempoEstado;
    unsigned long ultimoDisparo;
    unsigned long ultimoMisil;
    PuntoDebil puntosDebiles[3];

    // Colección de proyectiles del Boss (máximo 12 en pantalla)
    static const int MAX_PROYECTILES = 12;
    BossProyectil proyectiles[MAX_PROYECTILES];

    // Métodos internos de generación de ataques
    void dispararBalas();
    void dispararMisil(float objetivoX);
    void actualizarProyectiles();

public:
    Boss();
    void ejecutarAnimacionDestruccion(Adafruit_ILI9341& tft);
    void reset();
    void iniciarBatalla();
    void configurarPuntosDebiles();
    void actualizar(int playerX);
    void dibujar(Adafruit_ILI9341& tft);

    // Getters de estado
    EstadoBoss getEstado() const { return estado; }
    int getFase() const { return fase; }
    int getY() const { return y; }
    int getAlto() const { return alto; }

    // Procesa impacto de bala del jugador contra puntos débiles en sobrecalentamiento
    bool verificarImpactoBala(int balaX, int balaY);

    // Detección de colisiones contra la nave del jugador
    bool verificarColisionJugador(int playerX, int playerY);
    bool verificarImpactoProyectilJugador(int playerX, int playerY);
};

#endif // BOSS_H