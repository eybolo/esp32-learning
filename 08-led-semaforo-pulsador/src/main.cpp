#include <Arduino.h>

const int LED_VERDE = 33;
const int LED_AMARILLO = 25;
const int LED_ROJO = 26;
const int PULSADOR = 27;

const unsigned long TIEMPO_VERDE = 10000;
const unsigned long TIEMPO_AMARILLO = 3000;
const unsigned long TIEMPO_ROJO = 10000;

const unsigned long TIEMPO_MIN_VERDE = 3000;

unsigned long tiempoAnterior = 0;

bool peatonEsperando = false;
bool rojoExtendido = false;

enum Estado {
    VERDE,
    AMARILLO,
    ROJO
};

Estado estadoActual = VERDE;

void cambiarLuces(Estado estado){

switch (estado)
{
  case VERDE:
        digitalWrite(LED_VERDE, HIGH);
        digitalWrite(LED_AMARILLO, LOW);
        digitalWrite(LED_ROJO, LOW);
  break;

case AMARILLO:

        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARILLO, HIGH);
        digitalWrite(LED_ROJO, LOW);
  break;

case ROJO:

        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARILLO, LOW);
        digitalWrite(LED_ROJO, HIGH);
  break;
}
}


void setup() {
  Serial.begin(115200);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(PULSADOR, INPUT_PULLUP);

  cambiarLuces(estadoActual);

}
void loop()
{
    int leer_pulsador = digitalRead(PULSADOR);
    if (leer_pulsador == LOW && estadoActual == VERDE) {
        peatonEsperando = true;
    }

    Serial.print("Pulsador: ");
    Serial.println(leer_pulsador);

    switch (estadoActual)
    {
        case VERDE:

            if (
                (peatonEsperando == true &&
                millis() - tiempoAnterior >= TIEMPO_MIN_VERDE)
                ||
                (millis() - tiempoAnterior >= TIEMPO_VERDE)
            ) {
                estadoActual = AMARILLO;
                cambiarLuces(estadoActual);
                tiempoAnterior = millis();
                peatonEsperando = false;
            }

            break;


        case AMARILLO:

            if (millis() - tiempoAnterior >= TIEMPO_AMARILLO)
            {
                estadoActual = ROJO;
                cambiarLuces(estadoActual);
                tiempoAnterior = millis();
            }

            break;


        case ROJO:

            if (
                leer_pulsador == LOW &&
                millis() - tiempoAnterior >= TIEMPO_ROJO - 2000 &&
                millis() - tiempoAnterior <= TIEMPO_ROJO
            ) {
                rojoExtendido = true;
            }

            if (
                (rojoExtendido == false &&
                millis() - tiempoAnterior >= TIEMPO_ROJO)
                ||
                (rojoExtendido == true &&
                millis() - tiempoAnterior >= TIEMPO_ROJO + 3000)
            ) {
                estadoActual = VERDE;
                cambiarLuces(estadoActual);
                tiempoAnterior = millis();
                rojoExtendido = false;
            }

            break;
    }
}