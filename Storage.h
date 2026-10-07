#ifndef STORAGE_H
#define STORAGE_H

#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>

class StorageManager {
private:
    int chipSelectPin;
    bool sdDisponible;

public:
    StorageManager(int csPin);

    // Inicializa el bus SPI y la tarjeta MicroSD
    bool begin();

    // Carga los 5 mejores puntajes desde /scores.json
    void cargarScores(int topScores[5]);

    // Ordena e inserta el nuevo puntaje y sobreescribe /scores.json en la SD
    void guardarScore(int nuevoScore, int topScores[5]);

    // Verifica si la tarjeta SD está montada y funcional
    bool estaDisponible() const { return sdDisponible; }
};

#endif