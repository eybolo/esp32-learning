#include <Arduino.h>

const int FRECUENCIA = 5000;
const int RESOLUCION = 8;

int leds[] = {33, 25, 26};

void setup() {
  for (int i = 0; i < 3; i++) {
    ledcAttach(leds[i], FRECUENCIA, RESOLUCION);
  }
}

void loop() {

  for (int pin = 0; pin < 3; pin++) {

    for (int brillo = 0; brillo <= 255; brillo++) {
      ledcWrite(leds[pin], brillo);
      delay(20);
    }

    ledcWrite(leds[pin], 0);
  }
}