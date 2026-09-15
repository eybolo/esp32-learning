# Fade secuencial de 3 LEDs

## Objetivo

Controlar tres LEDs mediante PWM haciendo que aumenten
progresivamente su brillo uno después del otro.

## Hardware

- ESP32
- 3 LEDs
- 3 resistencias de 220 Ω
- Protoboard
- Jumpers

## Conexiones

| LED | GPIO | Resistencia | PWM |
|---|---:|---:|---:|
| LED 1 | 33 | 220 Ω | Canal 0 |
| LED 2 | 25 | 220 Ω | Canal 1 |
| LED 3 | 26 | 220 Ω | Canal 2 |

## Circuito

![Circuito](docs/circuito.png)

## Conceptos aprendidos

- PWM
- Duty cycle
- Resolución de 8 bits
- Canales LEDC
- Arrays
- Bucles for