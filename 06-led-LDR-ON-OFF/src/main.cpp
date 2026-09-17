#include <Arduino.h>

const int LDR_PIN = 32;

int leds[] = {33, 25, 26};

void setup(){
  Serial.begin(115200);
  for (int i=0; i<3; i++)
  {
    pinMode(leds[i],OUTPUT);
  }
}

void loop() {
  int medida = analogRead(LDR_PIN);

    Serial.print("LDR: ");
    Serial.println(medida);
    delay(500);
    for (int i = 0; i<3; i++)
    {
      if (medida < 1700)
      {
        digitalWrite(leds[i],HIGH);
      } else {
        digitalWrite(leds[i],LOW);
      }
  }
}