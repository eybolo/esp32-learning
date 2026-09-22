# Proyecto 08 --- Semáforo con pulsador peatonal

## Objetivo

Partiendo del semáforo con máquina de estados del proyecto anterior,
agregar un **pulsador peatonal** que pueda modificar el comportamiento
del semáforo.

El objetivo principal fue aprender a trabajar con una **entrada
digital** y utilizar eventos externos para tomar decisiones dentro de
una máquina de estados no bloqueante.

El proyecto fue probado primero en **Wokwi** y posteriormente montado y
probado físicamente con el ESP32.

## Componentes utilizados

-   ESP32 NodeMCU
-   3 LEDs: verde, amarillo y rojo
-   3 resistencias de 220 Ω
-   1 pulsador
-   Protoboard
-   Jumpers

## Pines utilizados

  Componente       GPIO
  -------------- ------
  LED verde          33
  LED amarillo       25
  LED rojo           26
  Pulsador           27

Cada LED utiliza su propia resistencia de **220 Ω**.

El pulsador se conecta así:

`GPIO 27 → Pulsador → GND`

No se utiliza una resistencia externa para el pulsador porque se utiliza
la resistencia **pull-up interna del ESP32**.

## Funcionamiento general

El semáforo tiene tres estados:

-   `VERDE`
-   `AMARILLO`
-   `ROJO`

Estos estados están definidos mediante un `enum`.

La secuencia normal es:

`VERDE → AMARILLO → ROJO → VERDE`

El programa continúa utilizando `millis()` en lugar de `delay()`, por lo
que el ESP32 puede seguir leyendo el pulsador mientras controla los
tiempos del semáforo.

## Tiempos utilizados

  Estado                    Duración
  -------------------- -------------
  Verde máximo           10 segundos
  Verde mínimo            3 segundos
  Amarillo                3 segundos
  Rojo                   10 segundos
  Extensión del rojo     +3 segundos

El verde tiene dos tiempos importantes:

-   **Tiempo mínimo:** 3 segundos.
-   **Tiempo máximo:** 10 segundos.

Esto permite atender una solicitud peatonal sin cambiar inmediatamente
el semáforo apenas comienza el verde.

## Entrada digital del pulsador

El pulsador utiliza:

`INPUT_PULLUP`

Esto activa una resistencia pull-up interna del ESP32.

Por lo tanto, la lógica del pulsador queda:

  Pulsador     GPIO
  ------------ --------
  Suelto       `HIGH`
  Presionado   `LOW`

Cuando el botón está suelto, la resistencia pull-up mantiene el GPIO en
un estado conocido (`HIGH`).

Cuando se pulsa el botón, el GPIO queda conectado a GND y pasa a `LOW`.

Esto evita que la entrada quede **flotante**.

## Pull-up y entradas flotantes

Una entrada digital necesita tener un nivel eléctrico definido.

Si un GPIO configurado como entrada no está conectado claramente a HIGH
ni a LOW, puede quedar **flotante** y producir lecturas impredecibles.

Una resistencia pull-up mantiene normalmente la entrada en HIGH.

Conceptualmente:

``` text
3.3 V
  │
 [R pull-up]
  │
  ├──── GPIO
  │
[Pulsador]
  │
 GND
```

En este proyecto no fue necesario colocar físicamente esa resistencia
porque el ESP32 dispone de una resistencia pull-up interna que puede
activarse mediante software.

Esto es diferente de las resistencias de 220 Ω de los LEDs.

Las resistencias de los LEDs se utilizan para **limitar corriente**,
mientras que una resistencia pull-up se utiliza para **definir el estado
lógico de una entrada**.

## `digitalRead()` vs `analogRead()`

En proyectos anteriores, como el LDR, se utilizó `analogRead()` porque
interesaba conocer un rango de valores provenientes de una señal
analógica.

Un pulsador solamente necesita representar dos estados:

-   presionado
-   no presionado

Por eso se utiliza `digitalRead()` y se obtiene:

-   `HIGH`
-   `LOW`

No necesitamos conocer un valor ADC intermedio.

## Guardar una solicitud peatonal

Una de las diferencias importantes aprendidas en este proyecto fue
distinguir entre:

**el estado físico actual del pulsador**

y

**una solicitud que ocurrió anteriormente y debe recordarse.**

Para recordar una solicitud se utiliza una variable booleana:

`peatonEsperando`

Puede tener solamente dos estados:

-   `false`: no existe una solicitud pendiente.
-   `true`: existe una solicitud peatonal.

Esto permite que el peatón pulse y suelte el botón sin necesidad de
mantenerlo presionado.

Por ejemplo:

1.  El semáforo lleva 1 segundo en verde.
2.  El peatón pulsa.
3.  `peatonEsperando` pasa a `true`.
4.  El peatón suelta el botón.
5.  El pulsador vuelve a `HIGH`.
6.  `peatonEsperando` continúa siendo `true`.
7.  Al llegar a los 3 segundos de verde, la solicitud puede ser
    atendida.

La variable funciona como una pequeña **memoria del evento ocurrido**.

## Comportamiento durante VERDE

El verde dura normalmente un máximo de 10 segundos.

Sin embargo, si un peatón pulsa el botón, se guarda la solicitud.

El semáforo solamente puede abandonar el verde por solicitud peatonal
cuando hayan transcurrido al menos 3 segundos.

Conceptualmente:

``` text
              VERDE
                │
       ┌────────┴─────────┐
       │                  │
 ¿hay solicitud?     ¿llegó a 10 s?
       │                  │
      Sí                 Sí
       │                  │
¿pasaron mínimo 3 s?      │
       │                  │
      Sí                  │
       └────────┬─────────┘
                │
                ▼
            AMARILLO
```

La transición ocurre cuando:

``` text
(solicitud peatonal Y mínimo de 3 segundos cumplido)
                         O
            (10 segundos cumplidos)
```

Aquí se utilizaron:

-   `&&` → AND
-   `||` → OR

De esta manera el semáforo continúa funcionando normalmente aunque nadie
utilice el pulsador.

## Consumir una solicitud

Una vez que el semáforo cambia de VERDE a AMARILLO debido a una
solicitud, ya no es necesario seguir recordándola.

Por eso `peatonEsperando` vuelve a `false`.

La solicitud ya fue atendida.

No necesitamos contar cuántos peatones pulsaron el botón. El sistema
solamente necesita saber:

> ¿Hay una solicitud que necesita ser atendida?

Si varias personas pulsan durante el verde, el resultado continúa siendo
simplemente:

`peatonEsperando = true`

## Comportamiento durante AMARILLO

El amarillo dura 3 segundos.

Las pulsaciones durante este estado se ignoran.

Esto se debe a que el semáforo ya está avanzando hacia ROJO, por lo que
el peatón ya está a punto de obtener el paso.

Después de 3 segundos:

`AMARILLO → ROJO`

## Comportamiento durante ROJO

El rojo dura normalmente 10 segundos.

Se agregó una segunda función al pulsador:

> Si un peatón pulsa durante los últimos 2 segundos del rojo, el rojo se
> extiende 3 segundos.

La ventana de extensión es:

``` text
0 s                         8 s       10 s
│────────────────────────────│─────────│
                             │         │
                             └─ botón ─┘
                                puede
                              extender
```

Si nadie pulsa:

`ROJO = 10 segundos`

Si alguien pulsa entre los segundos 8 y 10:

`ROJO = 13 segundos`

## Recordar la extensión del rojo

Para recordar si el rojo fue extendido se utiliza otra variable
booleana:

`rojoExtendido`

Inicialmente:

`rojoExtendido = false`

Si el botón se pulsa dentro de la ventana correspondiente:

`rojoExtendido = true`

Entonces el programa puede decidir entre dos tiempos:

``` text
rojoExtendido = false
        ↓
     10 segundos
        ↓
       VERDE
```

o:

``` text
rojoExtendido = true
        ↓
     13 segundos
        ↓
       VERDE
```

Al terminar el rojo, `rojoExtendido` vuelve a `false` para que la
extensión no afecte al siguiente ciclo.

## Una variable `bool` como memoria

Este proyecto permitió utilizar variables booleanas no solamente como
condiciones momentáneas, sino como **memoria del sistema**.

`peatonEsperando` recuerda:

> Anteriormente alguien solicitó cruzar.

`rojoExtendido` recuerda:

> Este ciclo de rojo debe durar 3 segundos adicionales.

Estas variables mantienen su valor aunque el `loop()` continúe
ejecutándose, hasta que el programa explícitamente las modifica.

## Un mismo evento puede tener diferentes efectos

Otro concepto importante aprendido es que un mismo evento puede producir
diferentes acciones dependiendo del estado actual de la máquina.

En este proyecto, pulsar el mismo botón significa:

``` text
VERDE
  │
  └── Solicitar anticipar el cambio a AMARILLO
      respetando el tiempo mínimo de verde.

AMARILLO
  │
  └── Se ignora.

ROJO
  │
  └── Si quedan 2 segundos o menos,
      extender el rojo 3 segundos.
```

Por lo tanto, no es el botón por sí solo el que decide qué sucede.

La decisión depende de:

**evento + estado actual**

## Estado físico vs evento recordado

El pulsador solamente informa qué está ocurriendo **ahora**:

``` text
HIGH → está suelto
LOW  → está presionado
```

Pero variables como `peatonEsperando` permiten recordar algo que ocurrió
anteriormente.

Por ejemplo:

``` text
Pulsador actual = HIGH
peatonEsperando = true
```

Esto es válido: el botón ya fue soltado, pero existe una solicitud
pendiente que todavía debe ser atendida.

## Rebote del pulsador

También se introdujo conceptualmente el problema del **rebote mecánico**
(*button bounce*).

Un pulsador físico puede producir pequeñas transiciones HIGH/LOW durante
unos milisegundos al hacer contacto.

En este proyecto no fue necesario implementar *debouncing* porque
repetir:

`peatonEsperando = true`

o:

`rojoExtendido = true`

no cambia el resultado.

Asignar `true` muchas veces sigue dejando la variable en `true`.

El *debouncing* y la detección de una única pulsación se estudiarán en
un proyecto futuro cuando exista un caso donde realmente sean
necesarios, por ejemplo un contador donde cada pulsación deba
contabilizarse exactamente una vez.

## Máquina de estados final

``` text
                         ┌───────────────┐
                         │     VERDE     │
                         │ máximo: 10 s  │
                         └───────┬───────┘
                                 │
                    ┌────────────┴────────────┐
                    │                         │
               nadie pulsa              peatón pulsa
                    │                         │
                espera 10 s          guarda solicitud
                                              │
                                      espera mínimo 3 s
                    │                         │
                    └────────────┬────────────┘
                                 ▼
                         ┌───────────────┐
                         │   AMARILLO    │
                         │      3 s      │
                         └───────┬───────┘
                                 ▼
                         ┌───────────────┐
                         │     ROJO      │
                         │     10 s      │
                         └───────┬───────┘
                                 │
                        durante 8–10 s
                                 │
                          ¿pulsan botón?
                           │           │
                          NO          SÍ
                           │           │
                         10 s        13 s
                           │           │
                           └─────┬─────┘
                                 ▼
                               VERDE
```

## Conceptos aprendidos

-   Entradas digitales.
-   `digitalRead()`.
-   `INPUT_PULLUP`.
-   Resistencias pull-up.
-   Entradas flotantes.
-   Diferencia entre INPUT y OUTPUT.
-   Diferencia entre una resistencia pull-up y una resistencia
    limitadora de corriente.
-   Lógica activa en LOW.
-   Variables `bool`.
-   Uso de variables como memoria.
-   Diferencia entre estado físico y evento recordado.
-   Condiciones con `&&` (AND).
-   Condiciones con `||` (OR).
-   Máquina de estados.
-   Eventos que modifican una máquina de estados.
-   Tiempo mínimo y máximo dentro de un estado.
-   Extensión dinámica de la duración de un estado.
-   Reinicio de variables al terminar un ciclo.
-   Uso no bloqueante de `millis()`.
-   Concepto de rebote mecánico de pulsadores.

## Resultado

El proyecto fue probado correctamente tanto en **Wokwi** como
posteriormente sobre el **ESP32 físico**.

El semáforo mantiene su funcionamiento automático, pero ahora puede
reaccionar a una entrada externa sin utilizar `delay()` ni bloquear la
ejecución del programa.

Este proyecto amplía la máquina de estados del proyecto anterior
incorporando por primera vez interacción física mediante una entrada
digital y memoria de eventos.
