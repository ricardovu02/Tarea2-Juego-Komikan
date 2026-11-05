# Tarea 2 - Torneo de Komikan Concurrente

**Integrantes:** Daniel Carmona y Ricardo Vargas
**Asignatura:** Sistemas Operativos

## Descripción

Este proyecto consiste en implementar un torneo concurrente del juego Komikan utilizando C++17 y la biblioteca POSIX Threads (pthread).

El programa recibe una cantidad de jugadores y un número máximo de mesas disponibles. Los jugadores llegan de manera concurrente, se forman parejas y cada pareja participa en una partida de Komikan.

Cada partida tiene sus propios hilos para controlar las fichas y un árbitro encargado de supervisar los eventos importantes. Las mesas pueden utilizarse nuevamente cuando una partida termina, respetando siempre el máximo de mesas simultáneas indicado al ejecutar el programa.

## Compilación

El proyecto está pensado para ejecutarse en Linux y necesita:

- Compilador g++ con soporte para C++17.
- Biblioteca pthread.
- Herramienta make.

Para compilar, ejecutar desde la carpeta principal:

```bash
make
```

Esto genera el ejecutable `torneo`.

Para limpiar los archivos generados:

```bash
make clean
```

## Ejecución

El programa se ejecuta con:

```bash
./torneo N M
```

Donde:

- `N` es la cantidad de jugadores que llegan al torneo.
- `M` es el número máximo de mesas que pueden estar ocupadas al mismo tiempo.

Por ejemplo:

```bash
./torneo 12 3
```

En este caso llegan 12 jugadores, se forman 6 parejas y pueden jugar hasta 3 partidas simultáneamente.

También se puede ejecutar con:

```bash
./torneo 2 1
```

Esto inicia una partida con una sola mesa disponible.

Si la cantidad de jugadores es impar, el último jugador que no consiga pareja queda sin participar.

## Configuración

El proyecto permite utilizar un archivo `config.txt` para establecer algunos parámetros de las partidas.

Entre ellos se encuentran las posiciones iniciales de las fichas, los retardos de movimiento y la semilla utilizada para la generación de valores aleatorios.

Si no se proporciona una configuración, se utilizan los valores predeterminados del programa.

Los retardos se aplican entre los movimientos de las fichas para simular su ejecución concurrente.

## Reglas del juego

Komikan se juega en un tablero de 5x5 con un trono adicional de tres posiciones.

Cada partida contiene:

- 1 Puma.
- 12 Perros.

El Puma puede desplazarse a posiciones conectadas que estén libres y capturar Perros saltando sobre ellos hacia una posición disponible.

Los Perros pueden realizar movimientos válidos por las conexiones del tablero, pero no pueden capturar al Puma.

Las condiciones de victoria son:

- **Victoria del Puma:** cuando consigue capturar 6 Perros.
- **Victoria de los Perros:** cuando el Puma queda inmovilizado y no tiene movimientos ni capturas disponibles.

Los movimientos se realizan automáticamente y no existen turnos fijos entre el Puma y los Perros.

## Organización del proyecto

El código está dividido en diferentes archivos para separar las responsabilidades.

```text
Tarea2-Juego-Komikan/
├── main.cpp
├── Makefile
├── config.txt
├── juego/
│   ├── juego.cpp
│   └── juego.h
├── partida/
│   ├── partida.cpp
│   └── partida.h
├── arbitro/
│   ├── arbitro.cpp
│   └── arbitro.h
├── matchmaking/
│   ├── torneo.cpp
│   └── torneo.h
└── README.md
```

**main.cpp:** recibe los argumentos, prepara la configuración e inicia el torneo.

**juego/:** contiene las funciones relacionadas con el tablero, los movimientos, las capturas y las condiciones de victoria.

**partida/:** administra el estado de cada partida y los hilos que controlan las fichas.

**arbitro/:** supervisa las capturas, comprueba el término de la partida y muestra los resultados.

**matchmaking/:** se encarga de recibir jugadores, formar parejas, administrar las mesas y controlar la ejecución general del torneo.

## Concurrencia y sincronización

Para implementar la concurrencia utilizamos hilos POSIX.

Cada partida cuenta con un hilo para el Puma, doce hilos para los Perros y un hilo árbitro.

Las fichas intentan moverse de manera independiente, utilizando retardos aleatorios entre movimientos. Antes de consultar o modificar el tablero, cada hilo debe obtener el mutex de su partida.

Esto permite evitar que dos fichas modifiquen las mismas posiciones al mismo tiempo y mantiene consistente el estado del tablero.

Los retardos se realizan fuera de la sección crítica para no mantener bloqueado el mutex mientras una ficha espera.

Para coordinar las capturas utilizamos una variable de condición llamada `avisoArbitro` y el estado `capturaPendiente`. Cuando el Puma captura un Perro, se avisa al árbitro para que registre el evento y muestre el tablero actualizado.

El administrador del torneo también utiliza un mutex para proteger las mesas disponibles y los contadores de ocupación.

Cuando todas las mesas están ocupadas, las parejas esperan mediante `pthread_cond_wait` hasta que alguna se libere. De esta forma no necesitan consultar constantemente si hay una mesa disponible.

Cada partida tiene su propio estado y sus mecanismos de sincronización, por lo que distintas mesas pueden funcionar de manera concurrente.

## Árbitro y salida del programa

Cada partida tiene un árbitro independiente que se encarga de informar los eventos importantes.

Para evitar una salida demasiado extensa, el tablero completo se muestra principalmente en los siguientes momentos:

- Al comenzar una partida.
- Después de cada captura realizada por el Puma.
- Al finalizar la partida por victoria o detención.

Los movimientos normales se muestran mediante mensajes breves que indican la ficha y las posiciones de origen y destino.

Cada evento identifica el número de mesa correspondiente, lo que permite distinguir las partidas cuando varias mesas están funcionando al mismo tiempo.

## Finalización y manejo de errores

El programa contempla la finalización normal de las partidas y también situaciones en las que deben detenerse.

Se implementó el manejo de SIGINT para permitir la interrupción del torneo mediante `Ctrl+C`.

Cuando el servidor recibe esta señal, solicita la detención de las partidas activas, despierta a los hilos que corresponda y evita iniciar nuevas partidas con las parejas pendientes.

También se revisan errores de creación de hilos. Si ocurre un fallo que impide continuar una partida, esta se detiene y se registra como error, sin declarar un ganador que no corresponda.

Además, existe una función `notificarAbandono()` que permite registrar el abandono de un jugador, terminar su partida y comunicar el resultado al árbitro. Este mecanismo funciona cuando se notifica el abandono; no corresponde a una detección automática de desconexiones externas.

## Resumen del torneo

Al finalizar la ejecución, el programa muestra un resumen que permite revisar lo ocurrido durante el torneo.

Este resumen incluye la cantidad de jugadores recibidos, las parejas formadas, las mesas utilizadas, los ganadores y las capturas de cada partida.

También indica cuántas partidas se realizaron en cada mesa, el máximo de mesas ocupadas simultáneamente y cuántas quedaron ocupadas al finalizar.

Esto permite comprobar que se respetó el límite de mesas y que los recursos fueron liberados correctamente.

## Pruebas realizadas

Durante el desarrollo realizamos pruebas con distintas cantidades de jugadores y mesas.

Entre los casos revisados se encuentran:

- Ejecución de una partida con 2 jugadores y 1 mesa.
- Ejecución con 7 jugadores y 1 mesa, comprobando el caso de jugadores impares y la reutilización de la mesa.
- Ejecución con 12 jugadores y 3 mesas, verificando las partidas simultáneas y la reutilización de recursos.
- Interrupción del torneo mediante SIGINT mientras existen partidas activas y parejas esperando.
- Simulación temporal de errores al crear los hilos del Puma, los Perros y el árbitro.
- Comprobación de las condiciones de victoria del Puma y de los Perros.
- Pruebas de notificación de abandono y registro del resultado correspondiente.

En la prueba con 12 jugadores y 3 mesas se formaron 6 parejas, se alcanzó el máximo de 3 mesas simultáneas y todas las partidas finalizaron con las mesas liberadas.

Las simulaciones utilizadas durante las pruebas de fallos se realizaron en archivos temporales, sin dejar mecanismos artificiales de prueba en el código final.

## Consideraciones finales

Para la sincronización de las fichas decidimos utilizar un solo mutex por partida. Aunque esto impide que dos movimientos de una misma partida modifiquen el tablero al mismo tiempo, facilita mantener la consistencia del juego, especialmente durante las capturas.

El sistema no garantiza que todos los hilos reciban el mismo tiempo de ejecución ni que todas las partidas terminen en un tiempo determinado, ya que los movimientos dependen de la planificación de los hilos y de las decisiones aleatorias.

El objetivo principal fue implementar un torneo concurrente que permitiera ejecutar varias partidas, coordinar correctamente los hilos, respetar el límite de mesas y finalizar liberando los recursos utilizados.
