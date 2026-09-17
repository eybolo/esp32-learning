# 05 - LED Fade con LDR

## Descripción

En este proyecto usamos una **fotoresistencia LDR** con un ESP32 para
detectar cambios de iluminación y utilizar esa información para
modificar el brillo de tres LEDs.

El objetivo principal no fue memorizar el código, sino entender el flujo
completo:

``` text
Luz → LDR → resistencia variable → divisor de tensión
     → voltaje → GPIO32 → ADC → valor digital → PWM → LEDs
```

> El código fuente del proyecto se encuentra en `src/main.cpp`. Este
> README funciona como documentación y apunte conceptual.

------------------------------------------------------------------------

## Componentes utilizados

-   ESP32 NodeMCU
-   Fotoresistencia LDR
-   Resistencia fija de 10 kΩ
-   3 LEDs
-   Resistencias de 220 Ω para los LEDs
-   Protoboard
-   Jumpers

------------------------------------------------------------------------

## 1. ¿Qué es una LDR?

**LDR** significa **Light Dependent Resistor**.

Es una resistencia cuyo valor cambia físicamente según la cantidad de
luz que recibe:

``` text
Más luz      → menor resistencia
Menos luz    → mayor resistencia
```

Este comportamiento pertenece al componente. No se configura mediante
software.

La LDR tampoco entrega directamente números como `700`, `2600` o `4095`.
Lo que hace físicamente es **cambiar su resistencia**.

------------------------------------------------------------------------

## 2. El divisor de tensión

Para poder detectar ese cambio con el ESP32 usamos la LDR junto con una
resistencia fija de 10 kΩ:

``` text
            3.3 V
              │
            [ LDR ]
              │
              ●──────── GPIO32
              │
           [ 10 kΩ ]
              │
             GND
```

La corriente principal circula por:

``` text
3.3 V → LDR → 10 kΩ → GND
```

GPIO32 está conectado al punto intermedio.

Una forma útil de recordarlo es:

> **GPIO32 monitorea el voltaje que existe entre la LDR y la resistencia
> de 10 kΩ.**

GPIO32 no mide la corriente.

------------------------------------------------------------------------

## 3. ¿Para qué sirve la resistencia de 10 kΩ?

La resistencia de 10 kΩ y la LDR forman un **divisor de tensión**.

La LDR es una resistencia variable y la de 10 kΩ es una resistencia
fija:

``` text
3.3 V
  │
 LDR       ← resistencia variable
  │
  ●──── GPIO32
  │
 10 kΩ     ← resistencia fija
  │
 GND
```

Las dos determinan físicamente qué voltaje existe en el punto central.

La resistencia de 10 kΩ **no es un número que el ADC utilice
posteriormente en un cálculo**. Cuando GPIO32 observa el punto central,
el voltaje ya está determinado por el circuito.

Sin la resistencia hacia GND no tendríamos el divisor de tensión
necesario para obtener una señal útil y variable.

------------------------------------------------------------------------

## 4. ¿Qué ocurre cuando cambia la luz?

Como en nuestro circuito la LDR está entre 3.3 V y GPIO32:

### Más luz

``` text
Más luz
  ↓
resistencia de la LDR disminuye
  ↓
el punto central queda más cerca de 3.3 V
  ↓
voltaje en GPIO32 aumenta
  ↓
lectura ADC aumenta
```

### Menos luz

``` text
Menos luz
  ↓
resistencia de la LDR aumenta
  ↓
la resistencia de 10 kΩ hacia GND tiene mayor influencia
  ↓
voltaje en GPIO32 disminuye
  ↓
lectura ADC disminuye
```

Durante las pruebas observamos aproximadamente:

  Condición                Lectura ADC
  ---------------------- -------------
  LDR tapada                  600--800
  Luz ambiente              2500--2700
  Linterna del celular      4000--4095

------------------------------------------------------------------------

## 5. GPIO32: el punto de monitoreo

GPIO32 funciona como una entrada analógica.

No decide qué voltaje debe existir ni realiza el divisor. Simplemente
observa el voltaje del nodo central respecto de GND:

``` text
3.3 V ── LDR ── ● ── 10 kΩ ── GND
                 │
               GPIO32
                 │
             monitorea
```

Podemos separar mentalmente las funciones:

``` text
LDR + 10 kΩ
     ↓
generan físicamente un voltaje variable

GPIO32
     ↓
monitorea ese voltaje
```

------------------------------------------------------------------------

## 6. ¿Qué función cumple GND?

GND no solamente permite cerrar el camino de corriente.

También es la **referencia de 0 V** utilizada para medir el voltaje.

Cuando decimos:

``` text
GPIO32 = 1.5 V
```

realmente queremos decir:

``` text
GPIO32 = 1.5 V respecto de GND
```

Por eso el voltaje siempre representa una diferencia entre dos puntos.

------------------------------------------------------------------------

## 7. ¿Qué es el ADC?

**ADC** significa **Analog to Digital Converter**.

El voltaje que existe físicamente en GPIO32 es una señal analógica. El
ADC la convierte en un número que el software puede utilizar:

``` text
Voltaje físico
     ↓
   GPIO32
     ↓
    ADC
     ↓
valor digital
```

En este proyecto las lecturas se representan aproximadamente entre:

``` text
0 ───────────────────────── 4095
↑                              ↑
lectura baja                lectura alta
```

Por ejemplo:

``` text
LDR: 2635
```

`2635` no son voltios, ohmios ni una unidad de iluminación.

Es una **representación digital de la medición realizada por el ADC**.

------------------------------------------------------------------------

## 8. De ADC a brillo

Una vez que el ADC obtiene un número, el software puede utilizarlo para
controlar los LEDs.

Tenemos dos rangos:

``` text
ADC:          0 ───────── 4095
PWM 8 bits:   0 ───────── 255
```

En este proyecto los relacionamos de forma inversa:

``` text
ADC bajo   → PWM alto → LED más brillante
ADC alto   → PWM bajo → LED más apagado
```

Por eso:

``` text
Oscuridad → ADC bajo → más brillo
Mucha luz → ADC alto → menos brillo
```

Para convertir un rango en otro utilizamos `map()` dentro del código.

------------------------------------------------------------------------

## 9. PWM

**PWM (Pulse Width Modulation)** permite controlar el brillo aparente de
los LEDs.

El ESP32 enciende y apaga el LED muy rápidamente. Modificando cuánto
tiempo permanece encendido dentro de cada ciclo obtenemos diferentes
niveles de brillo.

Con una resolución de 8 bits tenemos:

``` text
2^8 = 256 niveles

0 ... 255
```

Conceptualmente:

``` text
PWM 0   → apagado
PWM 128 → brillo intermedio
PWM 255 → brillo máximo
```

La frecuencia utilizada en el proyecto es de 5000 Hz, por lo que el
parpadeo es demasiado rápido para percibirlo normalmente.

`ledcAttach()` prepara cada GPIO para trabajar con PWM indicando el pin,
la frecuencia y la resolución.

`ledcWrite()` establece posteriormente el nivel PWM que queremos
aplicar.

------------------------------------------------------------------------

## 10. El gran problema: cómo funciona la protoboard

Durante gran parte del diagnóstico obteníamos:

``` text
LDR: 4095
LDR: 4095
LDR: 4095
```

Esto nos llevó a revisar GPIO, ADC, resistencias y montaje.

El aprendizaje clave fue entender que los agujeros de una protoboard no
son conexiones independientes.

En una protoboard típica:

``` text
A    B    C    D    E       F    G    H    I    J

●────●────●────●────●       ●────●────●────●────●
└── mismo metal ─────┘       └── mismo metal ─────┘
```

Por ejemplo:

``` text
50A = 50B = 50C = 50D = 50E
```

Todos pertenecen al mismo **nodo eléctrico**.

Pero:

``` text
50E ≠ 50F
```

porque el canal central separa ambos bloques.

### Regla aprendida

> **Si necesitamos que la corriente atraviese un componente, sus dos
> terminales deben estar conectados a nodos eléctricos diferentes.**

Si las dos patas de una resistencia están dentro del mismo grupo
conectado:

``` text
A50   B50   C50   D50   E50
 ●─────●─────●─────●─────●
       ↑           ↑
       └── 10kΩ ───┘
```

la tira metálica interna une sus extremos y la resistencia queda
puenteada.

En cambio:

``` text
NODO 1              NODO 2

●─────── [ LDR ] ───────●
```

obliga a que el componente quede entre dos nodos diferentes.

Esto puede lograrse usando filas diferentes o cruzando el canal central.

**No es obligatorio usar ambos lados de la protoboard. Lo importante es
pensar en nodos eléctricos, no solamente en agujeros físicos.**

------------------------------------------------------------------------

## 11. Pruebas de diagnóstico

Para encontrar el problema dejamos de modificar varias cosas
simultáneamente y aislamos cada parte.

### GPIO32 → GND

``` text
GPIO32 ───── GND
```

Resultado:

``` text
ADC = 0
```

### GPIO32 → 3.3 V

``` text
GPIO32 ───── 3.3 V
```

Resultado:

``` text
ADC = 4095
```

Esto permitió confirmar:

``` text
GPIO32       OK
ADC          OK
analogRead   OK
```

También comprobamos:

``` text
GPIO32 ── 10 kΩ ── GND
```

y obtuvimos una lectura cercana a `0`.

La conclusión importante fue que **aislar partes del circuito permite
distinguir un problema de software de uno eléctrico o de montaje**.

------------------------------------------------------------------------

## 12. Entrada flotante

Cuando dejamos un GPIO ADC desconectado aparecieron valores variables:

``` text
0
198
290
457
623
...
```

Esto ocurre porque la entrada queda **flotante**.

``` text
GPIO32 ───── X
```

Al no estar eléctricamente fijada a un nivel determinado, puede captar
ruido y producir valores aparentemente aleatorios.

Por eso:

> **Un ADC desconectado no tiene por qué leer 0.**

------------------------------------------------------------------------

## 13. No todos los GPIO son iguales

Durante las pruebas también aprendimos que no todos los GPIO del ESP32
tienen las mismas capacidades.

Algunos:

-   soportan ADC;
-   funcionan como entrada y salida;
-   son solamente entrada;
-   tienen funciones especiales;
-   participan en el arranque;
-   pueden estar asociados a otros periféricos.

En este proyecto finalmente utilizamos:

``` text
GPIO32 → lectura analógica de la LDR

GPIO33
GPIO25
GPIO26 → LEDs
```

La elección de un GPIO debe hacerse según la función que necesitamos.

------------------------------------------------------------------------

## 14. Metodología de diagnóstico aprendida

Cuando el circuito no funcionaba, la estrategia que finalmente nos
permitió avanzar fue:

``` text
1. comprobar GPIO + ADC
        ↓
2. comprobar GND
        ↓
3. comprobar 3.3 V
        ↓
4. comprobar resistencia
        ↓
5. comprobar divisor de tensión
        ↓
6. agregar nuevamente la LDR
```

La enseñanza es importante para futuros proyectos:

> **Cuando algo no funciona, cambiar una sola variable por vez y probar
> los componentes de forma aislada.**

------------------------------------------------------------------------

## 15. Modelo mental final

``` text
                 LUZ
                  ↓
                 LDR
                  ↓
       cambia su resistencia
                  ↓
          divisor de tensión
          LDR + resistencia 10k
                  ↓
          voltaje variable
                  ↓
               GPIO32
                  ↓
        monitorea el voltaje
                  ↓
                 ADC
                  ↓
            valor 0–4095
                  ↓
              software
                  ↓
             valor PWM
                  ↓
               LEDs
```

### Concepto para recordar

> **La LDR cambia su resistencia según la luz. La LDR y la resistencia
> fija de 10 kΩ convierten ese cambio en un voltaje variable. GPIO32
> monitorea ese voltaje. El ADC lo representa mediante un número digital
> y el software utiliza ese número para controlar el brillo de los
> LEDs.**

------------------------------------------------------------------------

## Temas para profundizar más adelante

-   Ley de Ohm aplicada al divisor de tensión.
-   Cálculo matemático del divisor de tensión.
-   Medición de voltaje con multímetro.
-   Calibración de sensores analógicos.
-   `map()` y `constrain()`.
-   ADC1 y ADC2.
-   Resolución y atenuación del ADC.
-   PWM y duty cycle.
