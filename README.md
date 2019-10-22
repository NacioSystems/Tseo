# Tseo de NacioSystems:

Versión 1.0
Fecha: 21/10/2019

Este proyecto es un robot llamado Tseo, que resuelve laberintos. Realizado por NacioSystems (O Milladoiro):

Introducción:

El robot está basado en la plataforma Pololu Zumo Arduino, al que se le incorporan tres sensores de distanciaVL6180, uno en el frontal, y otros dos a cada lado. Se incorpora también un encoder acoplado a la cadena de desplazamiento. Los sensores VL6180 están centrados en la caja soporte de baterías. Por último, aunque no necesario, se incorpora el sensor QTR frontal de Pololu Zumo, que se utiliza para leer la banda de entrada en meta.

El programa lee las paredes del laberinto a través de los sensores VL, con lo que puede determinar en que dirección se puede mover (no hay pared), hacia adelante, derecha, izquierda o dar la vuelta si es necesario. Con cuando avanza cuenta los pasos del encoder con lo que puede saber cuanto se mueve y determinar si ha avanzado una casilla. Por otra pare, para hacer los giros utiliza el giróscopo incluido en el Zumo, garantizando que los giros son completos y se encuentra en la orientación de destino. Como se indicó anteriormente, a través del sensor QTR frontal el robot puede saber cuando entra en la meta.


Tablero:

El laberinto está formado por un tablero cuadrado de 256 celdas cuadradas (16x16), cada una de ellas de 18cm de lado.
El tablero está organizado en dos ejes (X, Y), de tal manera que el eje X está orientado en el sentido 0º / 180º, y el eje Y está orientado en el sentido perpendicular 90º / 270º. Para entender esta orientación pensemos en la pantalla del ordenador, 0º sería hacia arriba, 180º hacia abajo, 90º hacia la derecha y 270º hacia la izquierda. Cada vez que el robot avanza, se desplaza en el eje X o en el eje Y, de tal manera que si avanza hacia 180º, el eje X se incrementa, mientras que si avanza hacia 0º el eje X de decremeta. Por otra parte si avanza hacia la dirección 90º el eje Y se incrementa, y si avanza hacia 270º el eje Y se decrementa.

Ejemplo de tablero y colocación de casillas (16,16), y orientación física:

X Y  00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15
   
00                              0º

   
01                              | Eje X
   
   
02                             180º
   
   
03
   
   
04
   
   
05 270º <--Eje Y--> 90º
   
   
06
   
   
07

   
08

   
09

   
10
  
   
11
 
   
12

   
13

   
14
  
   
15
   
   

Giros:

Cada vez que el robot gira a la derecha su orientación inicial se incrementa en 90º, mientras que si gira a la izquierda su orientación inicial se decrementa 90º. No hay que decir que cuando da media vuelta supone un giro de 180º, o lo que es lo mismo dos giros de 90º. 


Condiciones iniciales:

Se supone que la posición de inicio siempre es la misma, la casilla (0,0).
También se supone que la meta está siempre en el mismo sito, filas 7 y 8, columnas 7 y 8. La meta está cerrada totalmente a excepción de la entrada, que será siempre por la falta de muro entra las casillas (8,8) y (9,8).
El robot tendrá que saber en que orientación arranca, en sentido creciente del eje X o sentido creciente del eje Y, direcciones 180º o 90º respectivamente. Tseo utiliza el encoder, antes de inicio de la prueba, para indicarle la orientación de inicio, 180º por defecto, o 90º si se cambia.

Por otra parte Tseo, que no conoce el laberinto que va a resolver, puede elegir el sistema para recorrerlo y resolverlo entre tres sistemas, regla de la mano derecha, regla de la mano izquierda o por inundación (en fase de pruebas). Para ello se utiliza el botón del Zumo, dependiendo del tiempo de pulsación de dicho botón. Una pulsación corta inicia el movimiento en base a la regla de la mano derecha, una pulsación de más de 1 segundo hace que arranque con la regla de la mano izquierda, pero si se pulsa más de 3 segundos utiliza la técnica de la inundación.


Desplazamientos y giros:

Para los desplazamientos y giros se utilizan las rutinas de Pololu ZumoMotors y L3G TurnSensor. El robot se desplaza a la máxima velocidad que le permiten sus motores (de Minisumo), con la excepción de que tenga que hacer un giro, entonces reduce la velocidad en un 30% para compensar la inercia, que es bastante. Debido a su masa, en el momento que detecta el cambio de condiciones, por ejemplo un cruce de caminos, el robot recorre una distancia considerable después del frenazo, lo que dificulta situarse en la posición correcta para los giros. Para ello utiliza una velocidad menor y cuenta los pasos desde que toma la decisión de frenar para saber si está bien situado. Lo mismo ocurre en los giros, controlados por giróscopo, una vez completado el giro la inercia del robot hace que gire un poco de más, sobre 2º, por lo que, en función de la velocidad de giro, hay que completar ángulos más cortos, de unos 88º.


Programa:

El programa está realizado con el IDE Arduino, para su programación directa a través de cable USB. En la carpeta software se puede ver el programa comentado.


Construcción:

La construcción es muy sencilla, la base es la plataforma Pololu Zumo for Arduino, al que se le ha retirado la balda de Minisumo. La plataforma se complementa con un Arduino Uno, el sensor QTR del Zumo Refectance Sensor Array, el sensor de distancias VL6180 y el encoder HC-020K.

Lo único que hay que construir es el cableado de conexión de los VL6180 y el encoder al arduino, así como los soportes del encoder y los VL. El el programa para Arduino están indicadas las conexiones de cada pin.

Dependiendo de los motores que incorpore el Zumo, habrá que realizar ajustes a las velocidades para adaptarlas a la inercia del robot. En la fase de desarrollo me resultó imprescindible incorporarle un parachoques a Tseo, ya que las dimensiones de las cadenas hacen que en caso de derrapar o desplazamiento, las cadenas rocen con las paredes levantando al robot, creando un caos, incluso subiendo las paredes.


Piezas impresas:

En la carpeta de piezas impresas, están los modelos Stl del soporte del encoder, del parachoques protector y la carcasa de Tseo, que le dan esa personalidad tan destacada.


Referencias:

https://www.pololu.com/product/2506

https://store.arduino.cc/arduino-uno-rev3

https://www.pololu.com/product/1419

https://www.adafruit.com/product/3316

https://tienda.bricogeek.com/componentes/39-fotointerruptor-cnz1120.html

https://www.amazon.es/Velocidad-HC-020K-Medici%C3%B3n-fotoel%C3%A9ctrico-Encoders/dp/B00MA63ZCW

https://github.com/pololu/zumo-32u4-arduino-library/tree/master/examples/MazeSol

