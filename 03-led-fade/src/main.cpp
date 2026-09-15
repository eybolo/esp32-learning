#include <Arduino.h>

const int LED_PIN = 33;
const int CANAL = 0;
const int FRECUENCIA = 5000;
const int RESOLUCION = 8;

void setup() {
  ledcSetup(CANAL, FRECUENCIA, RESOLUCION);
  ledcAttachPin(LED_PIN, CANAL);
}

void loop() {

  for (int i=0; i<= 255; i++){
    ledcWrite(CANAL, i);
    delay(10);
  }
}