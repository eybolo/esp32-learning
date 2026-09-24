# Proyecto 10 --- Buzzer pasivo, PWM y notas musicales

## Objetivo

Aprender cómo generar sonidos y notas musicales con un buzzer pasivo
utilizando el ESP32 y el periférico LEDC.

En este proyecto se trabajó principalmente con:

-   buzzer activo y buzzer pasivo;
-   señales periódicas;
-   frecuencia y período;
-   PWM y duty cycle;
-   periférico LEDC del ESP32;
-   generación de tonos;
-   duración y separación entre notas;
-   creación de una función para reproducir notas;
-   relación entre frecuencia, notas musicales y octavas.

------------------------------------------------------------------------

## Componentes utilizados

-   ESP32 NodeMCU
-   Buzzer pasivo
-   Protoboard
-   Jumpers
-   Multímetro BRINNA HB-113L

También se utilizó Wokwi para realizar pruebas virtuales.

------------------------------------------------------------------------

## Circuito

La conexión utilizada fue sencilla:

``` text
GPIO27 ───── Buzzer pasivo ───── GND
```

El buzzer necesita recibir una señal que cambie periódicamente entre
HIGH y LOW para producir un tono.

------------------------------------------------------------------------

## Buzzer activo y buzzer pasivo

Durante las pruebas se compararon los dos tipos de buzzer.

### Buzzer activo

El buzzer activo posee internamente el circuito necesario para generar
su propia oscilación.

Por eso puede producir sonido al aplicarle una tensión continua.

``` text
3.3 V ──► buzzer activo ──► sonido
```

### Buzzer pasivo

El buzzer pasivo no genera por sí mismo la frecuencia necesaria.

Necesita recibir una señal periódica desde el ESP32:

``` text
HIGH → LOW → HIGH → LOW → ...
```

La frecuencia de esa señal determina el tono que escuchamos.

------------------------------------------------------------------------

## Frecuencia

La frecuencia indica cuántos ciclos completos ocurren en un segundo.

Su unidad es el hertz:

``` text
Hz = ciclos por segundo
```

Ejemplos:

``` text
500 Hz  = 500 ciclos por segundo
1000 Hz = 1000 ciclos por segundo
2000 Hz = 2000 ciclos por segundo
```

Al probar físicamente distintas frecuencias se observó:

``` text
500 Hz  → sonido más grave
1000 Hz → sonido intermedio
2000 Hz → sonido más agudo
```

Por lo tanto:

``` text
menor frecuencia → tono más grave
mayor frecuencia → tono más agudo
```

------------------------------------------------------------------------

## Período

El período representa cuánto tarda en completarse un ciclo.

La relación entre frecuencia y período es:

``` text
T = 1 / f
```

Por ejemplo:

``` text
500 Hz  → 2 ms por ciclo
1000 Hz → 1 ms por ciclo
2000 Hz → 0.5 ms por ciclo
```

Por lo tanto:

``` text
mayor frecuencia
       ↓
más ciclos por segundo
       ↓
menor período
```

El período no se configura directamente en este proyecto. Al indicar la
frecuencia, el hardware del ESP32 genera automáticamente el período
correspondiente.

------------------------------------------------------------------------

## PWM y duty cycle

Para generar la señal se utilizó PWM mediante LEDC.

Con una resolución de 8 bits, el duty cycle puede tomar valores entre:

``` text
0 ─────────────── 255
```

Se utilizó:

``` text
128 ≈ 50 %
```

Esto produce aproximadamente:

``` text
HIGH ────┐    ┌────
         │    │
LOW      └────┘
```

Para silenciar el buzzer se utilizó duty cycle `0`.

``` text
duty = 128 → señal PWM → sonido
duty = 0   → salida LOW → silencio
```

Es importante distinguir:

``` text
frecuencia → determina el tono
duty cycle → controla la proporción HIGH/LOW
```

------------------------------------------------------------------------

## LEDC

LEDC es un periférico de hardware del ESP32 capaz de generar señales
PWM.

En este proyecto se utilizaron principalmente tres operaciones:

### Configurar LEDC

``` cpp
ledcAttach(BUZZER_PIN, FRECUENCIA, RESOLUCION);
```

Configura inicialmente:

-   GPIO utilizado;
-   frecuencia;
-   resolución.

### Cambiar el duty cycle

``` cpp
ledcWrite(BUZZER_PIN, DUTY_CYCLE);
```

Permite activar la señal PWM.

Para generar silencio:

``` cpp
ledcWrite(BUZZER_PIN, 0);
```

### Cambiar la frecuencia

``` cpp
ledcChangeFrequency(BUZZER_PIN, nueva_frecuencia, RESOLUCION);
```

Permite modificar el tono mientras el programa está ejecutándose.

------------------------------------------------------------------------

## LEDC funciona independientemente del código

Una idea importante aprendida en este proyecto es que LEDC es un
periférico de hardware.

La CPU configura LEDC, pero después LEDC continúa generando la señal
PWM.

Por ejemplo:

``` cpp
ledcChangeFrequency(BUZZER_PIN, 440, RESOLUCION);
delay(2000);
```

Durante esos dos segundos, el código está detenido por `delay()`, pero
LEDC continúa generando la señal de 440 Hz.

Por eso el buzzer continúa sonando.

------------------------------------------------------------------------

## `delay()` y duración de una nota

`delay()` no significa directamente "hacer sonar el buzzer".

Por ejemplo:

``` cpp
ledcChangeFrequency(BUZZER_PIN, 440, RESOLUCION);
delay(2000);
```

ocurre así:

``` text
configurar 440 Hz
       ↓
LEDC genera 440 Hz
       ↓
el programa espera 2000 ms
       ↓
durante ese tiempo LEDC sigue funcionando
```

En la práctica, esto permite controlar cuánto tiempo permanece una nota
antes de ejecutar la siguiente instrucción.

------------------------------------------------------------------------

## Notas musicales

Una nota musical puede representarse mediante una frecuencia.

Se utilizaron, entre otras:

``` text
DO4  = 262 Hz
RE4  = 294 Hz
MI4  = 330 Hz
FA4  = 349 Hz
SOL4 = 392 Hz
LA4  = 440 Hz
SI4  = 494 Hz
DO5  = 523 Hz
```

Por ejemplo:

``` text
LA4 = 440 Hz
```

significa que para reproducir esa nota el buzzer recibe una señal de
aproximadamente 440 ciclos por segundo.

------------------------------------------------------------------------

## Octavas

También se aprendió la relación entre una nota y la misma nota una
octava superior.

Al subir una octava, la frecuencia se duplica.

Por ejemplo:

``` text
SOL3 = 196 Hz
SOL4 = 392 Hz
SOL5 = 784 Hz
```

Por lo tanto:

``` text
392 × 2 = 784 Hz
```

Aunque siguen siendo notas SOL, cada octava superior tiene una
frecuencia mayor y se escucha más aguda.

------------------------------------------------------------------------

## Frecuencia y duración

Para construir una melodía necesitamos principalmente dos datos:

``` text
nota       → frecuencia
duración   → tiempo
```

Por ejemplo:

``` text
LA4 durante 1000 ms
```

representa:

``` text
440 Hz durante 1 segundo
```

------------------------------------------------------------------------

## Función `tocar_nota()`

Durante el proyecto apareció código repetitivo para:

1.  activar el PWM;
2.  cambiar la frecuencia;
3.  esperar la duración de la nota;
4.  apagar el buzzer;
5.  generar una pequeña pausa.

Para evitar repetir toda esa lógica se creó una función propia:

``` cpp
void tocar_nota(int nota, int duracion) {
    ledcWrite(BUZZER_PIN, DUTY_CYCLE);
    ledcChangeFrequency(BUZZER_PIN, nota, RESOLUCION);
    delay(duracion);

    ledcWrite(BUZZER_PIN, 0);
    delay(100);
}
```

Esto permite expresar una nota de forma mucho más clara:

``` cpp
tocar_nota(LA4, 1000);
```

que significa:

``` text
frecuencia = 440 Hz
duración   = 1000 ms
```

La función oculta los detalles de LEDC y permite que el código de la
melodía sea más fácil de leer.

------------------------------------------------------------------------

## Silencio entre notas

También se comprobó que cambiar directamente de una frecuencia a otra no
genera silencio:

``` text
DO ───────── RE ───────── MI
```

Para separar las notas se apaga temporalmente el PWM:

``` cpp
ledcWrite(BUZZER_PIN, 0);
delay(100);
```

De esta forma se obtiene:

``` text
DO ───── silencio ───── RE ───── silencio ───── MI
```

Esto es especialmente útil cuando aparecen dos notas iguales
consecutivas, ya que permite distinguirlas.

------------------------------------------------------------------------

## Melodía

Como práctica final se reprodujo la melodía de **Cumpleaños feliz**.

Gracias a la función `tocar_nota()`, la melodía puede expresarse de
forma legible:

``` cpp
tocar_nota(SOL4, 250);
tocar_nota(SOL4, 250);
tocar_nota(LA4, 500);
tocar_nota(SOL4, 500);
tocar_nota(DO5, 500);
tocar_nota(SI4, 1000);
```

Esto se puede interpretar casi como una pequeña partitura:

``` text
SOL  SOL  LA  SOL  DO  SI
```

donde cada llamada contiene tanto la nota como su duración.

------------------------------------------------------------------------

## Arrays y `for`

También se realizó una prueba almacenando las notas y sus duraciones en
arrays:

``` cpp
int melodia[] = {SOL4, SOL4, LA4, SOL4, DO5, SI4};
int duraciones[] = {250, 250, 500, 500, 500, 1000};
```

Ambos arrays se relacionan mediante su índice:

``` text
índice            0      1      2      3      4      5

melodia[]        SOL4   SOL4    LA4   SOL4    DO5    SI4
duraciones[]      250    250     500    500    500   1000
```

Luego pueden recorrerse con:

``` cpp
for (int i = 0; i < 6; i++) {
    tocar_nota(melodia[i], duraciones[i]);
}
```

Esta técnica funcionó correctamente y permitió practicar arrays y bucles
`for`.

Sin embargo, para este proyecto se prefirió mantener las llamadas
explícitas a `tocar_nota()`, ya que hacen que la melodía sea más fácil
de leer.

Esto también dejó una enseñanza importante:

> Reducir la cantidad de líneas no siempre significa que el código sea
> más claro. Una abstracción es útil cuando mejora la organización o
> facilita entender y mantener el programa.

------------------------------------------------------------------------

## Problema encontrado con el buzzer físico

Durante las primeras pruebas, uno de los buzzers que se suponía pasivo
no producía sonido.

Se realizaron varias comprobaciones:

-   prueba con tensión continua;
-   inversión de polaridad;
-   conexión directa mediante jumpers;
-   medición de resistencia;
-   prueba de GPIO27 con `digitalWrite()`;
-   medición con multímetro directamente sobre el buzzer;
-   comprobación de niveles cercanos a 3 V y 0 V;
-   prueba del mismo programa en Wokwi.

El GPIO y el programa funcionaban correctamente.

Finalmente se probó otro buzzer recuperado de una PC y el circuito
funcionó.

Esto permitió concluir que el problema estaba probablemente en el
componente original o en su adecuación para esta aplicación, y no en la
lógica del programa ni en GPIO27.

Esta prueba también fue útil para aprender a separar un problema en:

``` text
software
hardware del ESP32
cableado
componente
```

y comprobar cada parte individualmente.

------------------------------------------------------------------------

## Conceptos aprendidos

Al finalizar este proyecto se practicaron y comprendieron:

-   diferencia entre buzzer activo y pasivo;
-   señales periódicas;
-   ciclos;
-   frecuencia en hertz;
-   período;
-   relación `T = 1/f`;
-   relación entre frecuencia y tono;
-   PWM;
-   duty cycle;
-   funcionamiento básico del periférico LEDC;
-   diferencia entre frecuencia y duty cycle;
-   funcionamiento de LEDC durante `delay()`;
-   generación de tonos;
-   frecuencias de notas musicales;
-   duración de notas;
-   silencios entre notas;
-   octavas y duplicación de frecuencia;
-   funciones con parámetros;
-   reutilización de código;
-   arrays;
-   índices desde cero;
-   recorrido de arrays con `for`;
-   diagnóstico de un componente físico utilizando multímetro y
    simulación.

------------------------------------------------------------------------

## Resultado final

El ESP32 quedó capaz de reproducir distintas notas y una melodía
mediante un buzzer pasivo.

El flujo conceptual final puede resumirse como:

``` text
nota musical
     ↓
frecuencia
     ↓
LEDC genera PWM
     ↓
GPIO27
     ↓
buzzer pasivo vibra
     ↓
sonido
```

Además:

``` text
frecuencia → tono
duración   → ritmo
silencio   → separación entre notas
```

Este proyecto permitió pasar de simplemente generar una señal PWM a
utilizarla para controlar un dispositivo físico y producir una secuencia
musical reconocible.
