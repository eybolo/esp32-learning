# ESP32 + LDR + 3 LEDs — Encendido automático según la luz

## 🎯 Objetivo

Construir un sistema con ESP32 capaz de detectar el nivel de luz ambiente mediante una fotoresistencia **LDR**.

El comportamiento buscado es:

- 💡 Hay suficiente luz → LEDs apagados.
- 🌑 Hay poca luz → LEDs encendidos.

Este proyecto representa el funcionamiento básico de sistemas como una **luz nocturna automática** o una iluminación exterior que se enciende cuando oscurece.

---

## 🧰 Componentes utilizados

- ESP32 NodeMCU ESP-32S
- 1 fotoresistencia LDR
- 1 resistencia de 10 kΩ para el divisor de tensión
- 3 LEDs
- 3 resistencias de 220 Ω
- Protoboard
- Jumpers

---

## 💡 ¿Qué es un LDR?

Un **LDR (Light Dependent Resistor)** es una resistencia cuyo valor cambia dependiendo de la cantidad de luz que recibe.

Esto permite utilizarlo como sensor para detectar cambios en la iluminación ambiente.

El ESP32 no mide directamente la resistencia del LDR. Para poder obtener una señal que pueda leer utilizamos el LDR junto con otra resistencia formando un **divisor de tensión**.

```text
3.3 V
  │
 [LDR]
  │
  ├──────── GPIO ADC del ESP32
  │
[10 kΩ]
  │
 GND
```

Cuando cambia la cantidad de luz que recibe el LDR, cambia su resistencia y como consecuencia cambia el voltaje en el punto conectado al ESP32.

---

## 📊 Del LDR al ADC

El ESP32 utiliza su **ADC (Analog-to-Digital Converter)** para convertir el voltaje analógico generado por el divisor de tensión en un número digital.

En nuestro caso trabajamos con valores dentro del rango:

```text
0 ───────────────────────────── 4095
```

El flujo es:

```text
Luz ambiente
     │
     ▼
    LDR
     │
     ▼
Cambia su resistencia
     │
     ▼
Cambia el voltaje
     │
     ▼
ADC del ESP32
     │
     ▼
Valor entre 0 y 4095
```

Esto permite que nuestro programa pueda tomar decisiones basándose en la cantidad de luz.

---

## 🔬 Mediciones realizadas

Antes de decidir cuándo encender los LEDs, realizamos mediciones reales del LDR.

### Con la luz de la habitación encendida

Obtuvimos aproximadamente:

```text
2640 - 2660
```

### Tapando el LDR

Obtuvimos aproximadamente:

```text
450 - 780
```

Esto nos permitió observar una separación bastante grande entre las dos situaciones:

```text
ADC
0                                              4095
│------------------------------------------------│

   OSCURIDAD                         LUZ
    450-780                       2640-2660
       ███                            █

                 ▲
                 │
                1700
               UMBRAL
```

---

## 🚦 El concepto de umbral

Uno de los conceptos nuevos de este proyecto fue el **umbral (threshold)**.

Un umbral es simplemente un valor que utilizamos como límite para tomar una decisión.

En nuestro proyecto elegimos inicialmente:

```text
UMBRAL = 1700
```

Entonces tenemos dos situaciones:

```text
Lectura ADC < 1700
        │
        ▼
   POCA LUZ
        │
        ▼
 LEDs ENCENDIDOS
```

y:

```text
Lectura ADC > 1700
        │
        ▼
  HAY SUFICIENTE LUZ
        │
        ▼
   LEDs APAGADOS
```

Es importante entender que **1700 no significa universalmente "oscuridad"**.

Ese número fue elegido basándonos en las mediciones realizadas con nuestro propio circuito.

En otro ambiente, con otro LDR o con diferentes condiciones de iluminación, podría ser necesario utilizar otro umbral.

---

## 🧠 Tomar decisiones con el ESP32

En proyectos anteriores utilizábamos los valores obtenidos por los sensores principalmente para observarlos o transformarlos.

En este proyecto comenzamos a utilizar esos datos para que el ESP32 **tome una decisión**.

La lógica completa es:

```text
              LUZ AMBIENTE
                    │
                    ▼
                   LDR
                    │
                    ▼
            Divisor de tensión
                    │
                    ▼
                ADC ESP32
                    │
                    ▼
             Valor 0 - 4095
                    │
                    ▼
            ¿Supera el umbral?
               /          \
             NO            SÍ
             │              │
             ▼              ▼
         POCA LUZ           LUZ
             │              │
             ▼              ▼
        LEDs ON         LEDs OFF
```

Para realizar esta decisión aprendimos a utilizar condiciones mediante **`if / else`**.

---

## 🔁 Arrays y bucles `for`

Como nuestro circuito tiene **3 LEDs**, también reforzamos el uso de arrays.

En lugar de tratar cada LED como algo completamente independiente, podemos guardar los GPIO de todos los LEDs dentro de un mismo array.

Conceptualmente:

```text
LEDs
 │
 ├── posición 0 → GPIO del LED 1
 ├── posición 1 → GPIO del LED 2
 └── posición 2 → GPIO del LED 3
```

Después podemos recorrerlos utilizando un bucle `for`.

Esto evita repetir las mismas instrucciones para cada LED y permite escribir programas más organizados y fáciles de ampliar.

También aprendimos la diferencia entre:

```text
i
```

que representa la posición dentro del array:

```text
0, 1, 2
```

y:

```text
leds[i]
```

que representa el GPIO almacenado en esa posición.

---

## ⚠️ Problema encontrado: GPIO 35

Durante las pruebas intentamos utilizar el **GPIO 35** para controlar uno de los LEDs.

El ESP32 comenzó a mostrar errores indicando que no podía utilizar ese GPIO como salida.

Descubrimos que, en el ESP32 clásico, algunos GPIO tienen capacidades especiales y **no todos pueden utilizarse de la misma manera**.

En particular:

```text
GPIO 34
GPIO 35
GPIO 36
GPIO 39
```

son **solo de entrada**.

Esto significa que pueden utilizarse para recibir información de sensores:

```text
Sensor
   │
   ▼
GPIO 35
   │
   ▼
ESP32
```

pero no para enviar una señal hacia un dispositivo:

```text
ESP32
   │
GPIO 35
   │
   X
   │
  LED
```

Para controlar un LED necesitamos un GPIO que pueda funcionar como **salida**.

Este problema nos enseñó algo importante:

> No alcanza con elegir cualquier número de GPIO. Antes de utilizar un pin del ESP32 debemos saber qué funciones soporta.

---

## 🔄 Diferencia con el proyecto anterior del LDR

En el proyecto anterior utilizamos el LDR para modificar progresivamente el **brillo de los LEDs**.

El flujo era aproximadamente:

```text
LDR
 │
 ▼
ADC
 │
 ▼
0 - 4095
 │
 ▼
map()
 │
 ▼
0 - 255
 │
 ▼
PWM
 │
 ▼
Brillo del LED
```

En este proyecto no necesitamos controlar diferentes niveles de brillo.

Solamente tenemos dos estados:

```text
             LDR
              │
              ▼
             ADC
              │
              ▼
            UMBRAL
           /      \
          /        \
        ON          OFF
```

Por eso para este proyecto ya no necesitamos trabajar con:

- frecuencia PWM
- resolución PWM
- `ledcAttach()`
- `map()`

Solo necesitamos decidir entre dos estados:

```text
HIGH → encendido
LOW  → apagado
```

---

## 🖥️ Simulación con Wokwi

También utilizamos **Wokwi** para simular el circuito.

Para representar nuestro NodeMCU ESP-32S seleccionamos el **ESP32 clásico**, en lugar de otras familias como:

```text
ESP32-S2
ESP32-S3
ESP32-C3
ESP32-C6
ESP32-H2
```

Estas son familias diferentes de microcontroladores y pueden tener distintos GPIO y características.

### Diferencia con el LDR físico

Nuestro LDR físico es un componente simple de **2 patas**.

En Wokwi encontramos un módulo de fotoresistencia que incluye:

```text
VCC
GND
DO
AO
```

La salida que nos interesa para este proyecto es:

```text
AO = Analog Output
```

porque queremos que el ESP32 reciba un valor analógico y utilice su ADC.

La salida:

```text
DO = Digital Output
```

no es necesaria para este ejercicio porque queremos que sea **nuestro programa** el que determine el umbral y decida cuándo encender los LEDs.

---

## 🔄 Flujo completo del proyecto

```text
                LUZ AMBIENTE
                     │
                     ▼
                    LDR
                     │
                     ▼
             DIVISOR DE TENSIÓN
                     │
                     ▼
              SEÑAL ANALÓGICA
                     │
                     ▼
                ADC DEL ESP32
                     │
                     ▼
                 0 - 4095
                     │
                     ▼
              COMPARAR UMBRAL
                     │
              ┌──────┴──────┐
              │             │
          POCA LUZ           LUZ
              │             │
              ▼             ▼
          LED 1 ON       LED 1 OFF
          LED 2 ON       LED 2 OFF
          LED 3 ON       LED 3 OFF
```

---

## 📚 Conceptos aprendidos

Durante este proyecto practicamos y reforzamos:

- Fotoresistencia **LDR**
- Divisor de tensión
- Señales analógicas
- ADC del ESP32
- Lecturas entre `0` y `4095`
- Uso de un **umbral**
- Condiciones `if / else`
- Comparaciones
- Estados `HIGH` y `LOW`
- Control digital de LEDs
- Arrays
- Bucles `for`
- Diferencia entre `i` y `leds[i]`
- GPIO de entrada y salida
- Limitaciones de algunos GPIO del ESP32
- Monitor serie
- Mediciones reales antes de elegir un umbral
- Simulación con Wokwi

---

## ✅ Resultado final

El sistema quedó funcionando correctamente.

Cuando apagamos las luces de la habitación:

```text
Menos luz
   ↓
Cambia el LDR
   ↓
Baja la lectura ADC
   ↓
Cruza el umbral
   ↓
Se encienden los 3 LEDs
```

Cuando volvemos a prender las luces:

```text
Más luz
   ↓
Cambia el LDR
   ↓
Sube la lectura ADC
   ↓
Supera el umbral
   ↓
Se apagan los 3 LEDs
```

Con este proyecto pasamos de simplemente **leer un sensor** a utilizar esa información para que el ESP32 **controle automáticamente una salida dependiendo de las condiciones del entorno**.