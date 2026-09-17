#include <Arduino.h>

const int FRECUENCIA = 5000;
const int RESOLUCION = 8;

const int FOTO = 32;
int leds[] = {33, 25, 26};

void setup() {

    Serial.begin(115200);

    for (int i = 0; i < 3; i++) {
        ledcAttach(leds[i], FRECUENCIA, RESOLUCION);
    }
}

void loop() {

    // Leemos la LDR
    int medida = analogRead(FOTO);

    // Convertimos ADC 0-4095 a brillo 255-0
    int brillo = map(medida, 0, 4095, 255, 0);

    Serial.print("LDR: ");
    Serial.print(medida);

    Serial.print(" | Brillo LED: ");
    Serial.println(brillo);

    // Aplicamos el mismo brillo a los 3 LEDs
    for (int i = 0; i < 3; i++) {
        ledcWrite(leds[i], brillo);
    }

    delay(200);
}