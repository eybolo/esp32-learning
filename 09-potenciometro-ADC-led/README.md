# Proyecto 09 --- Potenciómetro + ADC + control de brillo de un LED

## Objetivo

Aprender a utilizar un **potenciómetro como entrada analógica del
ESP32** y usar su posición para controlar el brillo de un LED.

El proyecto conecta varios conceptos:

``` text
Potenciómetro
     ↓
Voltaje analógico
     ↓
ADC del ESP32
     ↓
Valor 0–4095
     ↓
map()
     ↓
Valor 0–255
     ↓
PWM
     ↓
Brillo del LED
```

------------------------------------------------------------------------

## Componentes utilizados

-   ESP32 NodeMCU de 38 pines
-   Potenciómetro **B20K (20 kΩ)**
-   LED
-   Resistencia de **220 Ω**
-   Protoboard
-   Jumpers
-   Multímetro BRINNA HB-113L
-   PlatformIO + VS Code

------------------------------------------------------------------------

## Conexiones

### Potenciómetro

El potenciómetro tiene tres terminales:

-   un extremo → **3.3 V**
-   otro extremo → **GND**
-   terminal central (wiper) → **GPIO32**

Los dos extremos pueden intercambiarse. El circuito seguirá funcionando,
pero se invertirá el sentido en el que aumenta o disminuye el valor al
girar la perilla.

``` text
3.3 V ───────┐
             │
        [ Potenciómetro ]
             │
GPIO32 ── Wiper
             │
            GND
```

### LED

``` text
GPIO33 ── 220 Ω ── LED ── GND
```

El LED y el potenciómetro comparten el mismo GND del ESP32.

------------------------------------------------------------------------

# 1. Cómo funciona el potenciómetro

El potenciómetro B20K posee una pista resistiva de aproximadamente **20
kΩ** entre sus dos terminales exteriores.

Al medir físicamente el componente obtuvimos:

``` text
Extremo ↔ Extremo ≈ 19.55 kΩ
```

Este valor prácticamente no cambia al girar la perilla.

Esto ocurre porque el potenciómetro contiene **una única pista resistiva
continua**.

El terminal central se denomina **wiper** o cursor. Al girar la perilla,
el wiper se desplaza sobre esa pista.

El wiper permite considerar eléctricamente la pista como dos
resistencias:

``` text
Extremo A ─── R1 ─── Wiper ─── R2 ─── Extremo B

               R1 + R2 ≈ 20 kΩ
```

Por ejemplo:

``` text
R1 = 4 kΩ
R2 = 16 kΩ

R1 + R2 = 20 kΩ
```

Al girar la perilla cambian R1 y R2, pero la resistencia total entre los
extremos permanece aproximadamente constante.

------------------------------------------------------------------------

# 2. Potenciómetro como divisor de tensión

Conectando los extremos del potenciómetro entre 3.3 V y GND:

``` text
3.3 V
  │
 [R1]
  │
  ├──── Wiper
  │
 [R2]
  │
 GND
```

la pista resistiva queda sometida a una diferencia de potencial.

Circula una pequeña corriente por ella y se produce una caída de tensión
distribuida a lo largo de la pista.

El wiper permite acceder a un punto intermedio de esa caída de tensión.

Por eso:

-   cerca de GND → voltaje cercano a 0 V
-   aproximadamente en el centro → voltaje cercano a la mitad de la
    alimentación
-   cerca de 3.3 V → voltaje cercano al máximo

Un concepto importante aprendido durante el proyecto es que **el wiper
no crea la división de tensión**. La caída de tensión ya existe a lo
largo de la pista resistiva; el wiper simplemente permite acceder a
diferentes puntos de ella.

------------------------------------------------------------------------

# 3. Mediciones con el multímetro

Antes de utilizar el ADC se realizaron mediciones físicas.

### Resistencia total

``` text
Extremo ↔ Extremo ≈ 19.55 kΩ
```

Confirmamos que girar la perilla no modifica significativamente esta
resistencia.

### Resistencia entre wiper y extremo

Al medir entre el wiper y uno de los extremos, la resistencia sí cambia
al girar la perilla.

Esto permitió comprobar físicamente cómo el cursor divide la pista
resistiva.

### Voltaje del pin 3.3 V

Midiendo directamente:

``` text
3.3 V del ESP32 ↔ GND ≈ 3.079 V
```

Por lo tanto, el valor real de alimentación medido no era exactamente
3.300 V.

Al medir:

``` text
Wiper ↔ GND
```

el voltaje cambió al girar la perilla, desde valores cercanos a 0 V
hasta aproximadamente 3 V.

------------------------------------------------------------------------

# 4. ADC del ESP32

**ADC** significa **Analog-to-Digital Converter**.

Su función es convertir un voltaje analógico presente en un pin del
ESP32 en un número que pueda utilizar el programa.

En este proyecto utilizamos:

``` text
GPIO32
```

con una lectura ADC de 12 bits.

Una resolución de 12 bits proporciona:

``` text
2^12 = 4096 niveles
```

Por eso los valores posibles son:

``` text
0 ... 4095
```

Conceptualmente:

``` text
Voltaje bajo       → ADC cercano a 0
Voltaje intermedio → ADC cercano a 2048
Voltaje alto       → ADC cercano a 4095
```

Durante las pruebas físicas se observaron valores desde aproximadamente
`0` hasta `4095`.

Esto confirmó la cadena:

``` text
Girar potenciómetro
        ↓
Mover wiper
        ↓
Cambiar voltaje
        ↓
GPIO32 recibe ese voltaje
        ↓
ADC lo convierte en un número
```

------------------------------------------------------------------------

# 5. Conversión de ADC a brillo

El ADC trabaja con:

``` text
0 ... 4095
```

pero el PWM configurado a 8 bits trabaja con:

``` text
0 ... 255
```

Por eso utilizamos `map()` para relacionar ambos rangos:

``` text
ADC                 PWM

0       ──────────→ 0
1024    ──────────→ ~64
2048    ──────────→ ~128
3072    ──────────→ ~191
4095    ──────────→ 255
```

De esta manera, al aumentar el valor leído del potenciómetro también
aumenta el brillo del LED.

------------------------------------------------------------------------

# 6. PWM y control de brillo

Un GPIO digital normalmente trabaja con dos estados:

``` text
LOW
HIGH
```

Para conseguir niveles intermedios de brillo utilizamos **PWM (Pulse
Width Modulation)**.

En este proyecto se configuró:

``` text
Frecuencia: 5000 Hz
Resolución: 8 bits
```

Con 8 bits existen 256 niveles:

``` text
0 ... 255
```

PWM no genera simplemente un voltaje constante más pequeño.

En cambio, conmuta rápidamente la salida entre LOW y HIGH.

Por ejemplo, aproximadamente al 50 %:

``` text
HIGH  ┌────┐    ┌────┐    ┌────┐
      │    │    │    │    │    │
LOW ──┘    └────┘    └────┘    └──
         ~50 % duty
```

Como esto ocurre miles de veces por segundo, el ojo percibe un brillo
intermedio.

------------------------------------------------------------------------

# 7. Duty cycle

El **duty cycle** indica qué porcentaje de cada período PWM la señal
permanece en HIGH.

Con PWM de 8 bits:

``` text
PWM 0    →   0 % duty → LED apagado
PWM ~64  →  25 % duty
PWM ~128 →  50 % duty
PWM ~191 →  75 % duty
PWM 255  → 100 % duty → brillo máximo
```

Por lo tanto, PWM controla el brillo modificando **cuánto tiempo
permanece encendido el LED dentro de cada ciclo**, no reduciendo
simplemente el nivel HIGH del GPIO.

------------------------------------------------------------------------

# 8. Comprobación del PWM con el multímetro

Se midió el GPIO33 respecto de GND mientras el PWM estaba funcionando.

Resultados aproximados:

``` text
Duty ~25 %  → 0.76 V
Duty ~50 %  → 1.481 V
Duty 100 %  → 3.21 V
```

Esta prueba fue especialmente importante.

Por ejemplo, cuando el multímetro mostró aproximadamente:

``` text
1.48 V
```

el GPIO **no estaba produciendo 1.48 V constantes**.

En realidad estaba alternando rápidamente entre LOW y HIGH:

``` text
HIGH  ┌────┐    ┌────┐    ┌────┐
      │    │    │    │    │    │
LOW ──┘    └────┘    └────┘    └──
```

El multímetro en DC mostró aproximadamente el valor medio de esa señal.

Esto permitió comprobar físicamente la diferencia entre:

-   un voltaje analógico aproximadamente constante
-   una señal PWM cuyo valor medio puede ser parecido

Aunque el multímetro pueda mostrar valores similares, **no son la misma
señal**.

------------------------------------------------------------------------

# 9. Diferencia entre ADC y PWM

Este proyecto ayudó a diferenciar dos conceptos que pueden confundirse
al principio.

### ADC

Convierte una magnitud física analógica en información para el programa:

``` text
Voltaje analógico
       ↓
      ADC
       ↓
Número digital
```

### PWM

Hace el recorrido contrario desde el punto de vista del programa:
utiliza un número para controlar una señal física formada por pulsos.

``` text
Número del programa
       ↓
      PWM
       ↓
Pulsos eléctricos
       ↓
Brillo del LED
```

PWM **no es un DAC** y no genera directamente un voltaje analógico
constante.

------------------------------------------------------------------------

# 10. Flujo completo del proyecto

``` text
GIRAR POTENCIÓMETRO
        ↓
El wiper cambia de posición
        ↓
Cambia la proporción R1 / R2
        ↓
Cambia el voltaje Wiper ↔ GND
        ↓
GPIO32 recibe el voltaje
        ↓
ADC de 12 bits
        ↓
analogRead()
        ↓
0 ───────────── 4095
        ↓
map()
        ↓
0 ───────────── 255
        ↓
ledcWrite()
        ↓
PWM de 8 bits en GPIO33
        ↓
Duty cycle 0 % ───── 100 %
        ↓
LED apagado ───── brillo máximo
```

------------------------------------------------------------------------

# Conceptos aprendidos

En este proyecto se practicaron y relacionaron:

-   Potenciómetro
-   Pista resistiva
-   Wiper o cursor
-   Resistencia total y resistencia parcial
-   Divisor de tensión
-   Voltaje medido respecto de GND
-   Caída de tensión
-   ADC
-   Resolución en bits
-   ADC de 12 bits: 0--4095
-   `analogRead()`
-   Conversión de rangos con `map()`
-   PWM
-   Resolución PWM de 8 bits: 0--255
-   Frecuencia PWM
-   Duty cycle
-   `ledcAttach()`
-   `ledcWrite()`
-   Medición de señales PWM con multímetro
-   Diferencia entre voltaje analógico y PWM

------------------------------------------------------------------------

## Conclusión

Este proyecto permitió unir varios conceptos que hasta ahora se habían
estudiado por separado.

El potenciómetro genera una **entrada analógica variable** mediante un
divisor de tensión. El ADC convierte ese voltaje en un número de `0` a
`4095`. El programa transforma ese rango a `0–255` y finalmente el PWM
utiliza ese valor para controlar el brillo del LED.

La práctica con el multímetro permitió además comprobar físicamente que
una salida PWM no genera simplemente un voltaje intermedio constante: el
GPIO continúa alternando entre LOW y HIGH, y el duty cycle determina
durante cuánto tiempo permanece en cada estado.

El proyecto completa así el recorrido:

``` text
Mundo físico → ADC → Programa → PWM → Mundo físico
```
