#include <Arduino.h>

const int LED_VERDE = 33;
const int LED_AMARILLO = 25;
const int LED_ROJO = 26;

const unsigned long TIEMPO_VERDE = 3000;
const unsigned long TIEMPO_AMARILLO = 1000;
const unsigned long TIEMPO_ROJO = 3000;

unsigned long tiempoAnterior = 0;

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

  cambiarLuces(estadoActual);
}
void loop()
{
    switch (estadoActual)
    {
        case VERDE:

            if (millis() - tiempoAnterior >= TIEMPO_VERDE)
            {
                estadoActual = AMARILLO;
                cambiarLuces(estadoActual);
                tiempoAnterior = millis();
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

          if (millis() - tiempoAnterior >= TIEMPO_ROJO)
            {
                estadoActual = VERDE;
                cambiarLuces(estadoActual);
                tiempoAnterior = millis();
            }

            break;
    }
}

/*
switch (estadoActual)
{
  case VERDE:
        digitalWrite(LED_VERDE, HIGH);
        digitalWrite(LED_AMARILLO, LOW);
        digitalWrite(LED_ROJO, LOW);

        if (millis() - tiempoAnterior >= TIEMPO_VERDE)
        {
            estadoActual = AMARILLO;
            tiempoAnterior = millis();
        }
  break;

case AMARILLO:

        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARILLO, HIGH);
        digitalWrite(LED_ROJO, LOW);

        if (millis() - tiempoAnterior >= TIEMPO_AMARILLO)
        {
            estadoActual = ROJO;
            tiempoAnterior = millis();
        }
  break;

case ROJO:

        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARILLO, LOW);
        digitalWrite(LED_ROJO, HIGH);

        if (millis() - tiempoAnterior >= TIEMPO_ROJO)
        {
            estadoActual = VERDE;
            tiempoAnterior = millis();
        }
  break;
}
  */

/*
if (estadoActual == VERDE)
{
    digitalWrite(33, HIGH);
    digitalWrite(25, LOW);
    digitalWrite(26, LOW);

    if (millis() - tiempoAnterior >= 3000)
    {
        estadoActual = AMARILLO; 
        tiempoAnterior = millis();
    }
}

if (estadoActual == AMARILLO)
{
    digitalWrite(33, LOW);
    digitalWrite(25, HIGH);
    digitalWrite(26, LOW);

    if (millis() - tiempoAnterior >= 1000)
    {
        estadoActual = ROJO;
        tiempoAnterior = millis();
    }
}

if (estadoActual == ROJO)
{
    digitalWrite(33, LOW);
    digitalWrite(25, LOW);
    digitalWrite(26, HIGH);

    if (millis() - tiempoAnterior >= 3000)
    {
        estadoActual = VERDE; 
        tiempoAnterior = millis();
    }
}
}
*/