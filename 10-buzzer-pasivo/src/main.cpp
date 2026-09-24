#include <Arduino.h>

const int BUZZER_PIN = 27;
const int FRECUENCIA = 1000;
const int RESOLUCION = 8;
const int DUTY_CYCLE = 128;

const int DO4 = 262;
const int RE4 = 294;
const int RE5 = 587;
const int MI4 = 330;
const int FA4 = 349;
const int SOL4 = 392;
const int LA4 = 440;
const int SI4 = 494;
const int DO5 = 523;
const int MI5 = 659;
const int FA5 = 698;
const int SOL5 = 784;

void tocar_nota(int nota, int duracion) {
    ledcWrite(BUZZER_PIN, DUTY_CYCLE);
    ledcChangeFrequency(BUZZER_PIN, nota, RESOLUCION);
    delay(duracion);
    ledcWrite(BUZZER_PIN, 0);
    delay(100);
}

void setup() {
    ledcAttach(BUZZER_PIN, FRECUENCIA, RESOLUCION);
    ledcWrite(BUZZER_PIN, DUTY_CYCLE);
}

void loop() {
    // Primera frase
    tocar_nota(SOL4, 250);
    tocar_nota(SOL4, 250);
    tocar_nota(LA4, 500);
    tocar_nota(SOL4, 500);
    tocar_nota(DO5, 500);
    tocar_nota(SI4, 1000);

    // Segunda frase
    tocar_nota(SOL4, 250);
    tocar_nota(SOL4, 250);
    tocar_nota(LA4, 500);
    tocar_nota(SOL4, 500);
    tocar_nota(RE5, 500);
    tocar_nota(DO5, 1000);

    // Tercera frase
    tocar_nota(SOL4, 250);
    tocar_nota(SOL4, 250);
    tocar_nota(SOL5, 500);
    tocar_nota(MI5, 500);
    tocar_nota(DO5, 500);
    tocar_nota(SI4, 500);
    tocar_nota(LA4, 1000);

    // Cuarta frase
    tocar_nota(FA5, 250);
    tocar_nota(FA5, 250);
    tocar_nota(MI5, 500);
    tocar_nota(DO5, 500);
    tocar_nota(RE5, 500);
    tocar_nota(DO5, 1000);

    delay(2000);
}