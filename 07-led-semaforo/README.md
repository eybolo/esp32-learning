# 07 - Semáforo con ESP32

## Objetivo

Construir un semáforo utilizando tres LEDs controlados por un ESP32:

- 🟢 Verde
- 🟡 Amarillo
- 🔴 Rojo

El objetivo principal del proyecto no fue solamente encender los LEDs en secuencia, sino aprender a controlar tareas temporizadas **sin bloquear la ejecución del programa**.

Durante el proyecto se comenzó utilizando `delay()` y luego se reemplazó por `millis()`, llegando finalmente a implementar una pequeña **máquina de estados**.

---

## Hardware utilizado

- ESP32 NodeMCU ESP-32S
- Protoboard
- 3 LEDs:
  - Verde
  - Amarillo
  - Rojo
- 3 resistencias de 220 Ω
- Jumpers

---

## Conexiones

| LED | GPIO |
|---|---:|
| Verde | GPIO 33 |
| Amarillo | GPIO 25 |
| Rojo | GPIO 26 |

Cada LED utiliza su propia resistencia de 220 Ω.

Los tres LEDs comparten GND, pero cada uno constituye una rama diferente del circuito.

Conceptualmente:

GPIO → Resistencia → LED → GND

Para los tres LEDs:

GPIO 33 → 220 Ω → LED verde ─────┐
GPIO 25 → 220 Ω → LED amarillo ──┼── GND
GPIO 26 → 220 Ω → LED rojo ──────┘

Aunque comparten GND, cada rama tiene su propia corriente.

---

# Conceptos aprendidos

## 1. HIGH y LOW

Un GPIO configurado como salida puede colocarse en dos estados digitales principales:

- `HIGH`: aproximadamente 3.3 V
- `LOW`: aproximadamente 0 V

Por ejemplo, para mostrar el estado VERDE:

- LED verde → HIGH
- LED amarillo → LOW
- LED rojo → LOW

Enviar `HIGH` repetidamente a un GPIO que ya está en `HIGH` no hace que el LED parpadee.

Simplemente mantiene la salida en ese estado.

---

## 2. Primera versión utilizando delay()

La primera versión del semáforo utilizaba `delay()` para mantener cada luz durante determinado tiempo.

La secuencia era:

VERDE
  ↓
3 segundos
  ↓
AMARILLO
  ↓
1 segundo
  ↓
ROJO
  ↓
3 segundos
  ↓
VERDE...

Esta solución funciona correctamente para un programa sencillo.

El problema aparece cuando el ESP32 necesita realizar otras tareas mientras espera.

---

# El problema de delay()

Cuando se utiliza:

delay(3000);

el flujo normal del `loop()` queda detenido durante aproximadamente 3 segundos.

Durante ese tiempo el código que está después del `delay()` no se ejecuta.

Esto puede convertirse en un problema si el ESP32 necesita realizar otras tareas, por ejemplo:

- leer un pulsador
- leer un sensor
- actualizar una pantalla
- procesar comunicaciones
- realizar otras tareas del programa

Por esta razón se buscó una forma de medir el tiempo **sin detener el loop**.

---

# millis()

`millis()` devuelve la cantidad de milisegundos transcurridos desde que comenzó a ejecutarse el programa.

Por ejemplo:

millis() = 5000

significa aproximadamente:

5 segundos desde el inicio.

La llamada a `millis()` no espera.

Simplemente consulta el valor actual del reloj.

---

## Medir tiempo transcurrido

Se creó una variable:

unsigned long tiempoAnterior = 0;

Esta variable guarda el momento en el que comenzó el estado actual.

El tiempo transcurrido puede calcularse mediante:

millis() - tiempoAnterior

Ejemplo:

tiempoAnterior = 5000
millis()       = 8000

8000 - 5000 = 3000 ms

Por lo tanto pasaron 3 segundos.

---

## Por qué se utiliza >=

La comprobación se realiza utilizando:

millis() - tiempoAnterior >= tiempo

y no:

millis() - tiempoAnterior == tiempo

El `loop()` puede no ejecutar la comprobación exactamente en el milisegundo esperado.

Por ejemplo:

2999 ms → todavía no
3000 ms → podría no comprobarse exactamente aquí
3001 ms → ya pasó el tiempo

Con `>= 3000`, la condición sigue siendo verdadera aunque hayan pasado unos milisegundos adicionales.

---

# delay() vs millis()

## Con delay()

delay(3000)

El programa espera antes de continuar con el flujo normal del `loop()`.

## Con millis()

¿Pasaron 3000 ms?

NO → continuar ejecutando el programa  
NO → continuar ejecutando el programa  
NO → continuar ejecutando el programa  
SÍ → realizar la acción

El programa no queda esperando dentro del `if`.

Si la condición es falsa, continúa, termina esa vuelta del `loop()` y vuelve a comenzar.

Esto permite realizar otras tareas mientras se mide el tiempo.

---

# Estados

El semáforo puede encontrarse en uno de tres estados:

- VERDE
- AMARILLO
- ROJO

Un estado representa la situación completa del sistema en un determinado momento.

Por ejemplo:

VERDE:

LED verde     → HIGH  
LED amarillo  → LOW  
LED rojo      → LOW

Esto es diferente de decir que `HIGH` es el estado del semáforo.

`HIGH` y `LOW` representan niveles de los GPIO.

El **estado del sistema** es VERDE, AMARILLO o ROJO.

---

# enum

Para representar los estados se utilizó un `enum`.

Un `enum` permite definir un tipo con una lista determinada de valores posibles.

En este proyecto se creó el tipo:

Estado

con tres valores:

Estado
├── VERDE
├── AMARILLO
└── ROJO

Luego se creó una variable:

estadoActual

de tipo `Estado`.

Inicialmente:

estadoActual = VERDE

Esto permite que el código sea mucho más descriptivo que utilizar números como:

0 = verde  
1 = amarillo  
2 = rojo

Además, `Estado` es un tipo definido específicamente para nuestro programa.

---

# Máquina de estados

El funcionamiento del semáforo puede representarse como:

VERDE
  │
  │ 3000 ms
  ▼
AMARILLO
  │
  │ 1000 ms
  ▼
ROJO
  │
  │ 3000 ms
  ▼
VERDE
  │
  └──────── ciclo continuo

Cada cambio entre estados se denomina **transición**.

En este proyecto, las transiciones ocurren cuando transcurre determinada cantidad de tiempo.

---

# switch / case

Para ejecutar diferentes acciones dependiendo del estado actual se utilizó `switch`.

Conceptualmente:

switch (estadoActual)

├── case VERDE
│
├── case AMARILLO
│
└── case ROJO

El `switch` pregunta:

> ¿En qué estado se encuentra actualmente el sistema?

Después ejecuta solamente el `case` correspondiente.

`break` termina ese `case` y sale del `switch`.

En la siguiente vuelta del `loop()`, el `switch` vuelve a comprobar `estadoActual`.

---

# El loop no espera dentro del estado

Por ejemplo, si el estado actual es AMARILLO y solamente pasaron 500 ms:

500 >= 1000 → FALSE

El programa no espera dentro del `if`.

Ocurre:

case AMARILLO
      ↓
¿pasaron 1000 ms?
      ↓
     NO
      ↓
    break
      ↓
termina la vuelta del loop
      ↓
comienza otra vuelta
      ↓
case AMARILLO nuevamente

Esto ocurre rápidamente hasta que finalmente:

1000 >= 1000 → TRUE

y se realiza la transición a ROJO.

---

# Constantes para los GPIO

Inicialmente se utilizaron directamente números como:

33
25
26

Luego se reemplazaron por nombres descriptivos:

LED_VERDE
LED_AMARILLO
LED_ROJO

Esto hace que el código sea más fácil de leer y mantener.

Por ejemplo:

digitalWrite(LED_VERDE, HIGH);

es más descriptivo que:

digitalWrite(33, HIGH);

Además, si en el futuro cambia un GPIO, solamente es necesario modificar su constante.

---

# Constantes para los tiempos

También se eliminaron los tiempos escritos directamente dentro de la lógica.

Se definieron valores descriptivos para:

- TIEMPO_VERDE
- TIEMPO_AMARILLO
- TIEMPO_ROJO

Esto evita tener números como `3000` o `1000` repartidos por el programa.

También permite modificar fácilmente la duración de un estado desde un único lugar.

---

# Función cambiarLuces()

Se creó una función propia encargada de configurar físicamente los LEDs según el estado recibido.

Conceptualmente:

cambiarLuces(VERDE)

configura:

verde     → HIGH
amarillo  → LOW
rojo      → LOW

Mientras que:

cambiarLuces(ROJO)

configura:

verde     → LOW
amarillo  → LOW
rojo      → HIGH

La función recibe un parámetro de tipo `Estado`.

Esto permitió separar dos responsabilidades:

## cambiarLuces()

Se ocupa de:

> ¿Cómo deben estar físicamente los LEDs?

## loop()

Se ocupa de:

> ¿En qué estado estoy y cuándo debo pasar al siguiente?

---

# Estado lógico vs salida física

Una idea importante aprendida durante el proyecto fue diferenciar:

estadoActual = AMARILLO

de:

cambiarLuces(AMARILLO)

La primera instrucción modifica el **estado lógico del programa**.

La segunda modifica las **salidas físicas del ESP32** mediante `digitalWrite()`.

Cambiar una variable no enciende automáticamente un LED.

El programa debe traducir ese estado lógico en señales eléctricas hacia los GPIO.

---

# Inicialización en setup()

El estado inicial del programa es VERDE.

Sin embargo, asignar:

estadoActual = VERDE

solamente modifica una variable.

Por eso, después de configurar los GPIO como `OUTPUT`, se llama a la función que configura físicamente las luces.

Conceptualmente:

ESP32 arranca
      ↓
estadoActual = VERDE
      ↓
setup()
      ↓
configurar GPIO como OUTPUT
      ↓
cambiarLuces(estadoActual)
      ↓
LED verde encendido
      ↓
loop()

Como `setup()` se ejecuta una sola vez, es un lugar apropiado para establecer el estado físico inicial.

---

# Evitar digitalWrite() innecesarios

En una primera versión de la máquina de estados, los `digitalWrite()` estaban dentro de cada `case`.

Como el `loop()` se ejecuta continuamente, eso provocaba que se ejecutaran muchas veces:

HIGH → HIGH → HIGH → HIGH...

Esto no hacía parpadear el LED porque el GPIO seguía manteniendo el mismo estado, pero era innecesario.

La versión final ejecuta `cambiarLuces()` solamente cuando se produce una transición.

Por ejemplo:

VERDE
  │
  │ pasan 3 segundos
  ▼
estadoActual = AMARILLO
  ↓
cambiarLuces(estadoActual)

De esta manera los GPIO se modifican solamente cuando es necesario.

---

# Arquitectura final

La lógica final del proyecto puede resumirse así:

                estadoActual
                     │
                     ▼
             switch(estadoActual)
                     │
          ┌──────────┼──────────┐
          ▼          ▼          ▼
       VERDE      AMARILLO     ROJO
       3000 ms     1000 ms    3000 ms
          │          │          │
          ▼          ▼          ▼
      AMARILLO      ROJO       VERDE
          │          │          │
          └──────┬───┴──────────┘
                 ▼
          cambiarLuces()
                 │
                 ▼
           digitalWrite()
                 │
                 ▼
              LEDs

---

# Flujo general aprendido

El proyecto evolucionó progresivamente:

1. Controlar tres LEDs mediante GPIO.
2. Crear la secuencia del semáforo.
3. Temporizar utilizando `delay()`.
4. Entender el problema del código bloqueante.
5. Aprender `millis()`.
6. Medir tiempo mediante una diferencia.
7. Crear estados.
8. Crear transiciones entre estados.
9. Utilizar `enum`.
10. Organizar los estados mediante `switch/case`.
11. Crear constantes para GPIO y tiempos.
12. Crear una función propia.
13. Separar la lógica de control de las salidas físicas.
14. Construir una máquina de estados no bloqueante.

---

# Conclusión

Este proyecto comenzó como un simple semáforo de tres LEDs, pero terminó sirviendo como introducción a una técnica fundamental en sistemas embebidos: las **máquinas de estados no bloqueantes**.

La diferencia principal respecto a proyectos anteriores es que el programa ya no necesita detenerse para controlar el tiempo.

El `loop()` puede continuar ejecutándose mientras se comprueba cuánto tiempo pasó.

Esto permitirá que futuros proyectos puedan realizar varias tareas simultáneamente desde el punto de vista del programa, por ejemplo:

- controlar un semáforo
- detectar un pulsador
- leer sensores
- reaccionar a eventos

sin depender de largos `delay()`.

## Próximo paso

Una continuación natural de este proyecto es agregar un **pulsador peatonal** al semáforo.

Eso permitirá aplicar de forma práctica la principal ventaja de haber reemplazado `delay()` por `millis()`: poder detectar eventos mientras el semáforo continúa funcionando.