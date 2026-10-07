#include "Storage.h"

StorageManager::StorageManager(int csPin) {
    chipSelectPin = csPin;
    sdDisponible = false;
}

bool StorageManager::begin() {
    Serial.print("Iniciando MicroSD en PIN ");
    Serial.print(chipSelectPin);
    Serial.print("... ");
    if (SD.begin(chipSelectPin)) {
        sdDisponible = true;
        Serial.println("OK!");
        return true;
    }
    sdDisponible = false;
    Serial.println("Fallo! (Modo sin SD activo)");
    return false;
}

void StorageManager::cargarScores(int topScores[5]) {
    // Inicializar arreglo en 0 por defecto
    for (int i = 0; i < 5; i++) {
        topScores[i] = 0;
    }
    if (!sdDisponible) return;

    File file = SD.open("/scores.json", FILE_READ);
    if (!file) {
        Serial.println("No se encontro /scores.json, se creara uno nuevo al guardar.");
        return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, file);
    if (!error) {
        for (int i = 0; i < 5; i++) {
            topScores[i] = doc["scores"][i] | 0;
        }
    } else {
        Serial.println("Error al deserializar /scores.json");
    }
    file.close();
}

void StorageManager::guardarScore(int nuevoScore, int topScores[5]) {
    // 1. Insertar el nuevo puntaje en una lista temporal de 6 elementos
    int tempScores[6];
    for (int i = 0; i < 5; i++) {
        tempScores[i] = topScores[i];
    }
    tempScores[5] = nuevoScore;

    // 2. Ordenamiento de burbuja de mayor a menor (idéntico a codigo juego.txt)
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 6; j++) {
            if (tempScores[j] > tempScores[i]) {
                int temp = tempScores[i];
                tempScores[i] = tempScores[j];
                tempScores[j] = temp;
            }
        }
    }

    // 3. Copiar los mejores 5 de vuelta al arreglo principal
    for (int i = 0; i < 5; i++) {
        topScores[i] = tempScores[i];
    }

    // 4. Imprimir tabla en consola Serial (idéntico al código original)
    Serial.println("=== TOP 5 SCORES ===");
    for (int i = 0; i < 5; i++) {
        Serial.print(i + 1);
        Serial.print(". ");
        Serial.println(topScores[i]);
    }

    if (!sdDisponible) return;

    // 5. Eliminar el archivo antiguo para asegurar una escritura limpia sin corrupción
    if (SD.exists("/scores.json")) {
        SD.remove("/scores.json");
    }

    // 6. Serializar e importar el JSON a la SD
    StaticJsonDocument<256> doc;
    JsonArray array = doc.createNestedArray("scores");
    for (int i = 0; i < 5; i++) {
        array.add(topScores[i]);
    }

    File file = SD.open("/scores.json", FILE_WRITE);
    if (file) {
        serializeJson(doc, file);
        file.close();
        Serial.println("Scores guardados exitosamente en MicroSD.");
    } else {
        Serial.println("Error al abrir /scores.json para escritura.");
    }
}