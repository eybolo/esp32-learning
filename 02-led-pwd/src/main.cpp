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

  ledcWrite(CANAL, 255);  // 100%
  delay(2000);

  ledcWrite(CANAL, 128);  // ~50%
  delay(2000);

  ledcWrite(CANAL, 30);   // ~12%
  delay(2000);

  ledcWrite(CANAL, 0);    // apagado
  delay(2000);

}