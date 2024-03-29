# Tseo
## de NacioSystems:

Versión 2.0
Fecha: 21/10/2019

Este proyecto es un robot llamado Tseo, que resuelve laberintos. Realizado por NacioSystems (O Milladoiro):

- - - 

### Introducción:

El robot está basado en la plataforma **Pololu Zumo Arduino**, al que se le incorporan tres sensores de distancia _VL6180_, uno en el frontal, y otros dos a cada lado. Se incorpora también un encoder acoplado a la cadena de desplazamiento. Los sensores _VL6180_ están centrados en la caja soporte de baterías. Por último, aunque no necesario, se incorpora el sensor _QTR_ frontal de Pololu Zumo, que se utiliza para leer la banda de entrada en meta.

El programa lee las paredes del laberinto a través de los sensores VL, con lo que puede determinar en que dirección se puede mover (no hay pared), hacia adelante, derecha, izquierda o dar la vuelta si es necesario. Cuando avanza cuenta los pasos del encoder con lo que puede saber cuanto se mueve y determinar si ha avanzado una casilla. Por otra parte, para hacer los giros utiliza el giróscopo _L3G_ incluido en el Zumo, garantizando que los giros son completos y se encuentra en la orientación de destino. Como se indicó anteriormente, a través del sensor _QTR_ frontal el robot puede saber cuando entra en la meta.

![Tseo](https://github.com/NacioSystems/Tseo/blob/master/Imagenes/TSO2%20con%20encoders.jpg "Tseo listo")

### Materiales:
* Pololu Zumo for Arduino (sin balda)
* Arduino UNO
* 3 x VL6180
* Zumo Reflectance Sensor Array (QTR)
* Encoder HC-020K, con rueda ranurada
* Piezas impresas 3D PLA
* Tornillos de 2mm de diámetro para sujeción Arduino y eje rueda ranurada
* Cables para conexionado, (recomendado CI perforado universal para soldar cables)


### Tablero:

El laberinto está formado por un tablero cuadrado de 256 celdas cuadradas (16x16), cada una de ellas de 18cm de lado.
El tablero está organizado en dos ejes (X, Y), de tal manera que el eje X está orientado en el sentido 0º / 180º, y el eje Y está orientado en el sentido perpendicular 90º / 270º. Para entender esta orientación pensemos en la pantalla del ordenador, 0º sería hacia arriba, 180º hacia abajo, 90º hacia la derecha y 270º hacia la izquierda. Cada vez que el robot avanza, se desplaza en el eje X o en el eje Y, de tal manera que si avanza hacia 180º, el eje X se incrementa, mientras que si avanza hacia 0º el eje X de decremeta. Por otra parte si avanza hacia la dirección 90º el eje Y se incrementa, y si avanza hacia 270º el eje Y se decrementa.

**Ejemplo de tablero y colocación de casillas (16,16), y orientación física:**

![Tablero](https://github.com/NacioSystems/Tseo/blob/master/Imagenes/Tablero.JPG "Tablero laberinto")
   

### Giros:

Cada vez que el robot gira a la derecha su orientación inicial se incrementa en 90º, mientras que si gira a la izquierda su orientación inicial se decrementa 90º. No hay que decir que cuando da media vuelta supone un giro de 180º, o lo que es lo mismo dos giros de 90º. 


### Condiciones iniciales:

Se supone que la posición de inicio siempre es la misma, la casilla (0,0).
También se supone que la meta está siempre en el mismo sito, filas 7 y 8, columnas 7 y 8. La meta está cerrada totalmente a excepción de la entrada, que será siempre por la falta de muro entre las casillas (8,8) y (9,8).
El robot tendrá que saber en que orientación arranca, en sentido creciente del eje X o sentido creciente del eje Y, direcciones 180º o 90º respectivamente. Tseo utiliza el encoder, antes de inicio de la prueba, para indicarle la orientación de inicio, 180º por defecto, o 90º si se cambia.

Por otra parte Tseo, que no conoce el laberinto que va a resolver, puede elegir el sistema para recorrerlo y resolverlo entre tres opciones; regla de la mano derecha, regla de la mano izquierda o por inundación (en fase de pruebas). Para ello se utiliza el botón del Zumo, dependiendo del tiempo de pulsación de dicho botón. Una pulsación corta inicia el movimiento en base a la regla de la mano derecha, una pulsación de más de 1 segundo hace que arranque con la regla de la mano izquierda, pero si se pulsa más de 3 segundos utiliza la técnica de la inundación.


### Desplazamientos y giros:

Para los desplazamientos y giros se utilizan las rutinas de Pololu ZumoMotors y L3G TurnSensor. El robot se desplaza a la máxima velocidad que le permiten sus motores (de Minisumo), con la excepción de que tenga que hacer un giro, entonces reduce la velocidad en un 30% para compensar la inercia, que es bastante. Debido a su masa, en el momento que detecta el cambio de condiciones, por ejemplo un cruce de caminos, el robot recorre una distancia considerable después del frenazo, lo que dificulta situarse en la posición correcta para los giros. Para ello utiliza una velocidad menor y cuenta los pasos desde que toma la decisión de frenar para saber si está bien situado. Lo mismo ocurre en los giros, controlados por giróscopo, una vez completado el giro la inercia del robot hace que gire un poco de más, sobre 2º, por lo que, en función de la velocidad de giro, hay que completar ángulos más cortos, de unos 88º.


### Programa:

El programa está realizado con el IDE Arduino, para su programación directa a través de cable USB. En la carpeta software se puede ver el programa comentado. El pineado está explicado en los comentarios, sólo es necesario montar el cableado para el encoder y los sensores VL, utilizando los pines de expansión que hay en la placa Zumo. En mi caso utilicé pines hembra en la parte exterior de la *shield* y en el puerto de expansión frontal.

Una vez configurados todos los sensores y sistemas de Tseo, el programa admite el cambio de configuración por defecto, ya sea la selección de la regla mano derecha, mano izquierda, o la conmutación de los ejes del laberinto. La selección de la regla de inicio es a través de una pulsación larga del botón de inicio. Por defecto sale en el eje *X*, pero si el laberinto es diferente se selecciona la salida en el eje *Y* mediante un giro de la rueda del encoder antes del inicio. Tseo responderá con unos pitidos de que ya está configurado.

Pulsando el botón de inicio, una pulsación corta, empieza a resolver el laberinto, de tal forma que cada vez que encuentra un cruce decide tomar una decisión que guarda en la memoria. Lo mismo en el caso de que llegue a una pared y no pueda seguir, para dar media vuelta. Cada vez que se da media vuelta sabe que ese camino que recorrió desde la última decisión es incorrecto, por lo que elimina de la memoria ese camino cambiando la penúltima decisión tomada, por otra más adecuada. De esa manera va recorriendo el laberinto y guardando las decisiones tomadas, cuando detecta que en su camino el sensor de líneas delantero reconoce el color blanco, sabe que llegó a la meta. Tambien sabe que está en la meta por la casilla en la que se encuentra, pues através del _encoder_ y los giros sabe en que posición está. Una vez en la meta se para y espera que lo coloquen en la casila de salida de nuevo para iniciar de nuevo el recorrido, esta vez por el camino más corto, teniendo en cuenta las decisiones que fue almacenando en la memoria.

Tseo podría volver sólo a la casilla de origen recorriendo el camino inverso que tiene guardado en la memoria, pero dependiendo de la complegidad del laberinto, y el tiempo de competición que resta, parece más rápido y adecuado llevarlo directamente en brazos al origen. Esta función se implementará en la siguiente versión, de tal manera que si no se recoge de la meta en un tiempo determinado, Tseo decida volver el solo.

Tseo no necesita guardar todos los giros y avances que hace en la memoria, lo que le permite ahorrar mucha RAM y dedicarla a otros asuntos. Esto es, en el caso de que esté recorriendo un pasillo sin ningún cruce, aunque tenga que hacer giros a derecha o izquierda, independientemente de las casillas que avance, seguirá el único camino posible pero no tomará nota de los giros, ya que no aportan información para resolver el labertinto.

Cuando la decisión a tomar es un giro, comprueba su orientación respecto de la marcación en grados del giróscopo y hace girar los motores uno en un sentido y el otro en el inverso, dependiendo si es un giro a derechas o a izquierdas. Va leyendo la marcación del giróscopo y hasta que no alcanza los 90º de giro, mantiene los motores en marcha.

Cuando avanza va comprobando a través de los sensores VL si tiene o no pared a la derecha, izquierda o delante, de esa manera sabe si llegó a un cruce o todavía está pasando el cruce al que había llegado.

Al final, después de recorrer todo el laberinto, hace un pequeño baile mientras toca una alegre canción.

Por otra parte, a través del encoder, que está conectado a las interrupciones (pin 2), va contando los pasos que avanza, de tal manera que recorrida la longitud de una casilla y sabiendo la orientación en la que está, actualiza su posición. De esa manera sabe en que casilla está en cada momento, y reconoce por su situación cuando está en la casilla de meta. Esta funcionalidad está pensada para utilizar el sistema de resolución por _inundación_, todavía en evaluación por las capacidades y potencia del **ATmega328P** del Arduino. Otros robots que trabajan por la regla de la _inundación_ disponen de procesadores más rápidos y potentes, hacerlo con un **ATmega328P** a 16 MHz es todo un reto.


### Construcción:

La construcción es muy sencilla, la base es la plataforma **_Pololu Zumo for Arduino_**, al que se le ha retirado la balda de Minisumo. La plataforma se complementa con un **_Arduino Uno_**, el sensor **QTR** del **_Zumo Refectance Sensor Array_**, los tres sensores de distancias **_VL6180_** y el encoder **_HC-020K_**.

![Esquema de conexión](https://github.com/NacioSystems/Tseo/blob/master/Imagenes/Conexiones.JPG "Esquema de conexión")

Lo único que hay que construir es el cableado de conexión de los **VL6180** y el encoder al Arduino, así como los soportes del encoder y los VL. *_La Pololu Zumo Shield for Arduino_* se encarga de realizar las conexiones con el resto de componentes, teniendo en cuenta que deberemos dejar los puentes de lectura de la tensión de la batería (A1) y del interfaz **_I2C_** (SCL, SDA), necesarios para comunicarse con el giróscopo **L3G** y los sensores **VL6180** a través de la librería _Whire_.  Para la conexión de la tarjeta _Bluetooth 5V_ es necesario montar tambien una tira de 4 pines hembra en el panel de expansión frontal izquierdo de la _shield_.  El el programa para Arduino están indicadas las conexiones de cada pin.

![Esquema Zumo Arduino](https://github.com/NacioSystems/Tseo/blob/master/Imagenes/Esquema%20Zumo.JPG "Esquema Pololu Zumo Arduino")

Dependiendo de los motores que incorpore el Zumo, habrá que realizar ajustes a las velocidades para adaptarlas a la inercia del robot. En la fase de desarrollo me resultó imprescindible incorporarle un parachoques a Tseo, ya que las dimensiones de las cadenas hacen que en caso de derrapar o desplazamiento, las cadenas rocen con las paredes levantando al robot, creando un caos, incluso subiendo las paredes.

![Tseo en construcción](https://github.com/NacioSystems/Tseo/blob/master/Imagenes/Montaje%20Tseo.jpg "Tseo en construcción")

La carcasa incluye un hueco muy práctico para instalar el interface _Bluetooth_ (HL05 - HL06), por si se quiere hacer un seguimiento del funcionamiento desde un terminar remoto.

También incluye un orificio para instalar un mástil, para llevar la bandera de Tseo, con la que se paseará de modo triunfante por el laberinto. El mástil puede ser un palillo de madera de los utilizados para las brochetas. La bandera está impresa en papel.


### Piezas impresas:

En la carpeta de piezas impresas, están los modelos Stl del soporte del encoder, del parachoques protector y la carcasa de Tseo, que le dan esa personalidad tan destacada.

![Carcasa Tseo](https://github.com/NacioSystems/Tseo/blob/master/Imagenes/Carcasa%20Tseo.JPG "Carcasa Tseo")

![Brazo encoder](https://github.com/NacioSystems/Tseo/blob/master/Imagenes/Brazo%20encoder.JPG "Brazo Encoder")


### Referencias:

* *[Pololu Zumo for Arduino][1]*

* *[Aduino UNO R3][2]*

* *[Pololu Zumo Refectance Sensor Array][3]*

* *[Sensor LIDAR VL6180][4]*

* *[Encoder CNZ1120][5]*

* *[Encoder HC-020k][6]*

* *[Ejemplos Pololu][7]*

* *[Reglamento Laberinto OSHWDEM][8]*

### Autor:
**Ignacio Otero**

### Agradecimientos:

Muchas gracias **Bea** por tu colaboración en la revisión y corrección de esta página.

### Licencia:
Todos estos productos están liberados mediante Creative Commons Attribution-ShareAlike 4.0 International License.

[1]:https://www.pololu.com/product/2506

[2]:https://store.arduino.cc/arduino-uno-rev3

[3]:https://www.pololu.com/product/1419

[4]:https://www.adafruit.com/product/3316

[5]:https://tienda.bricogeek.com/componentes/39-fotointerruptor-cnz1120.html

[6]:https://www.amazon.es/Velocidad-HC-020K-Medici%C3%B3n-fotoel%C3%A9ctrico-Encoders/dp/B00MA63ZCW

[7]:https://github.com/pololu/zumo-shield

[8]:http://rules.oshwdem.org/labirinto_es

