# Tseo
Proyecto de robot resuelve laberintos de NacioSystems:

El robot est� basado en la plataforma Pololu Zumo Arduino, al que se le incorporan tres sensores de distanciaVL6180, uno en el frontal, y otros dos a cada lado. Se incorpora tambi�n un encoder acoplado a la cadena de desplazamiento. Los sensores VL6180 est�n centrados en la caja soporte de bater�as. Por �ltimo, aunque no necesario, se incorpora el sensor QTR frontal de Pololu Zumo, que se utiliza para leer la banda de entrada en meta.

El programa lee las paredes del laberinto a trav�s de los sensores VL, con lo que puede determinar en que direcci�n se puede mover (no hay pared), hacia adelante, derecha, izquierda o dar la vuelta si es necesario. Con cuando avanza cuenta los pasos del encoder con lo que puede saber cuanto se mueve y determinar si ha avanzado una casilla. Por otra pare, para hacer los giros utiliza el gir�scopo incluido en el Zumo, garantizando que los giros son completos y se encuentra en la orientaci�n de destino. Como se indic� anteriormente, a trav�s del sensor QTR frontal el robot puede saber cuando entra en la meta.

Tablero:
El laberinto est� formado por un tablero cuadrado de 256 celdas cuadradas (16x16), cada una de ellas de 18cm de lado.
El tablero est� organizado en dos ejes (X, Y), de tal manera que el eje X est� orientado en el sentido 0� / 180�, y el eje Y est� orientado en el sentido perpendicular 90� / 270�. Para entender esta orientaci�n pensemos en la pantalla del ordenador, 0� ser�a hacia arriba, 180� hacia abajo, 90� hacia la derecha y 270� hacia la izquierda. Cada vez que el robot avanza, se desplaza en el eje X o en el eje Y, de tal manera que si avanza hacia 180�, el eje X se incrementa, mientras que si avanza hacia 0� el eje X de decremeta. Por otra parte si avanza hacia la direcci�n 90� el eje Y se incrementa, y si avanza hacia 270� el eje Y se decrementa.

Ejemplo de tablero y colocaci�n de casillas (16,16), y orientaci�n f�sica:

X Y  00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+ 
00 |                             0�                                |
   +   +   +   +   +   +   +   + /\+   +   +   +   +   +   +   +   + 
01 |                             | Eje X                           |
   +   +   +   +   +   +   +   + \/+   +   +   +   +   +   +   +   + 
02 |                            180�                               |
   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   + 
03 |                                                               |
   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   + 
04 |                                                               |
   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   + 
05 |                     270� <--Eje Y--> 90�                      |
   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   + 
06 |                                                               |
   +   +   +   +   +   +   +   +---+---+   +   +   +   +   +   +   + 
07 |                           |       |                           |
   +   +   +   +   +   +   +   +   M   +   +   +   +   +   +   +   + 
08 |                           |       |                           |
   +   +   +   +   +   +   +   +---+   +   +   +   +   +   +   +   + 
09 |                                                               |
   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   + 
10 |                                                               |
   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   + 
11 |                                                               |
   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   + 
12 |                                                               |
   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   + 
13 |                                                               |
   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   + 
14 |                                                               |
   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   + 
15 |                                                               |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

Giros:
Cada vez que el robot gira a la derecha su orientaci�n inicial se incrementa en 90�, mientras que si gira a la izquierda su orientaci�n inicial se decrementa 90�. No hay que decir que cuando da media vuelta supone un giro de 180�, o lo que es lo mismo dos giros de 90�. 

Condiciones iniciales
Se supone que la posici�n de inicio siempre es la misma, la casilla (0,0).
Tambi�n se supone que la meta est� siempre en el mismo sito, filas 7 y 8, columnas 7 y 8. La meta est� cerrada totalmente a excepci�n de la entrada, que ser� siempre por la falta de muro entra las casillas (8,8) y (9,8).
El robot tendr� que saber en que orientaci�n arranca, en sentido creciente del eje X o sentido creciente del eje Y, direcciones 180� o 90� respectivamente. Tseo utiliza el encoder, antes de inicio de la prueba, para indicarle la orientaci�n de inicio, 180� por defecto, o 90� si se cambia.

Por otra parte Tseo, que no conoce el laberinto que va a resolver, puede elegir el sistema para recorrerlo y resolverlo entre tres sistemas, regla de la mano derecha, regla de la mano izquierda o por inundaci�n (en fase de pruebas). Para ello se utiliza el bot�n del Zumo, dependiendo del tiempo de pulsaci�n de dicho bot�n. Una pulsaci�n corta inicia el movimiento en base a la regla de la mano derecha, una pulsaci�n de m�s de 1 segundo hace que arranque con la regla de la mano izquierda, pero si se pulsa m�s de 3 segundos utiliza la t�cnica de la inundaci�n.

Desplazamientos y giros:
Para los desplazamientos y giros se utilizan las rutinas de Pololu ZumoMotors y L3G TurnSensor. El robot se desplaza a la m�xima velocidad que le permiten sus motores (de Minisumo), con la excepci�n de que tenga que hacer un giro, entonces reduce la velocidad en un 30% para compensar la inercia, que es bastante. Debido a su masa, en el momento que detecta el cambio de condiciones, por ejemplo un cruce de caminos, el robot recorre una distancia considerable despu�s del frenazo, lo que dificulta situarse en la posici�n correcta para los giros. Para ello utiliza una velocidad menor y cuenta los pasos desde que toma la decisi�n de frenar para saber si est� bien situado. Lo mismo ocurre en los giros, controlados por gir�scopo, una vez completado el giro la inercia del robot hace que gire un poco de m�s, sobre 2�, por lo que, en funci�n de la velocidad de giro, hay que completar �ngulos m�s cortos, de unos 88�.

Construcci�n:
La construcci�n es muy sencilla, la base es la plataforma Pololu Zumo for Arduino, al que se le ha retirado la balda de Minisumo. La plataforma se complementa con un Arduino Uno, el sensor QTR del Zumo Refectance Sensor Array, el sensor de distancias VL6180 y el encoder HC-020K.

Lo �nico que hay que construir es el cableado de conexi�n de los VL6180 y el encoder al arduino, as� como los soportes del encoder y los VL. 

Dependiendo de los motores que incorpore el Zumo, habr� que realizar ajustes a las velocidades para adaptarlas a la inercia del robot. En la fase de desarrollo me result� imprescindible incorporarle un parachoques a Tseo, ya que las dimensiones de las cadenas hacen que en caso de derrapar o desplazamiento, las cadenas rocen con las paredes levantando al robot, creando un caos, incluso subiendo las paredes.

Piezas impresas:
En la carpeta de piezas impresas, est�n los modelos Stl del soporte del encoder, del parachoques protector y la carcasa de Tseo, que le dan esa personalidad tan destacada.

Referencias:
https://www.pololu.com/product/2506
https://store.arduino.cc/arduino-uno-rev3
https://www.pololu.com/product/1419
https://www.adafruit.com/product/3316
https://tienda.bricogeek.com/componentes/39-fotointerruptor-cnz1120.html
https://www.amazon.es/Velocidad-HC-020K-Medici%C3%B3n-fotoel%C3%A9ctrico-Encoders/dp/B00MA63ZCW
https://github.com/pololu/zumo-32u4-arduino-library/tree/master/examples/MazeSol
