#include <Arduino.h>

const int potenciometro_pin = 32;
const int led_pin = 33;
const int FRECUENCIA = 5000;
const int RESOLUCION = 8;

void setup() {
  Serial.begin(115200);
  pinMode(potenciometro_pin,INPUT);

  // Configura el PWM: pin, frecuencia y resolución
  ledcAttach(led_pin, FRECUENCIA, RESOLUCION);
}

void loop() {

    // Lee el voltaje del potenciómetro mediante el ADC (0 a 4095)
    int medida_potenciometro = analogRead(potenciometro_pin);

    // Convierte el rango del ADC de 12 bits al rango del PWM de 8 bits
    int brillo = map(medida_potenciometro, 0, 4095, 0, 255);

    // Controla el duty cycle del PWM
    ledcWrite(led_pin, brillo);

    Serial.print("Potenciometro: ");
    Serial.println(medida_potenciometro);
    delay(1000);
}
