// RESUELVE LABERINTOS
// VERSIÓN 8.0 29/10/2018
// Por Ignacio Otero
// Ayuda de la idea de Pololu

// PINES USADOS EN ESTA VERSIÓN (19)
// BlueTooth y Serial (2) = 0, 1
// Motores Zumo (4) = 7, 8, 9, 10
// Sensor IR Zumo (7) = A0, A2, A3, 2, 4, 5, 11
// Buzzer Zumo (1) = 3 
// LED sólo en el setup, es compartido con Shutdown VLD (1) = 13 
// Pulsador Pushbutton Zumo (1)= 12
// Shutdown de sensores LIDAR VL medidores de distancia (1) = 6, 13
// Batería Zumo (1) = A1 afectado por jumper de batería
// I2C (2) = A4, A5
// Lector de interrupciones 0 por pin 2 para contar los pasos
// Si es necesario se puede utilizar A1 desconectando el jumper de la batería
// En caso de necesidad podría utilizarse los pines 7 y 8 (MDir1/2)
// como salidas digitales, siempre con motores parados o QTRSensor desactivado.

// CONFIGURACIÓN

// Para manejar el giróscopo L3G necesitamos I2G, librerias Wire
#include <Wire.h>
#include <L3G.h> // Consume 13 bytes
#include <TurnSensor.h> /* Esta librería requiere que se denomine
                        la clase L3G como "gyro", ya que es el nombre
                        que utiliza. Consume 69 bytes.
                        Se elimina de la librería TurnSensor el encendido del Led
                        Nombres de variables o constantes utilizados que no aparecen 
                        en el programa pero vienen definidos en las librerías:

                        ZUMMO_BUTTON es el nombre del botón del Zumo, pin 12

                        turnAngle    es una varible de la librería TurnSensor que
                                     indica el ángulo de giro relativo
                                     desde la medición anterior
          
                        turnAngle1   es una constante que representa el valor
                                     del giro de aproximadamente un ángulo de 1 grado
                                     Pololu recomienda que para un ángulo de 45º, se
                                     representa por el valor 0x20000000. 
                                     Un grado serían 45 veces menos.
           
                        turnRate     es la velocidad de giro instantánea, respecto de la
                                     velocidad anterior, en unidades de 0,07 grados por s.
                                     Se representa entre -180º y 180º, en sentido contrario
                                     a las agujas del reloj
                                     */

#include <VL6180X.h>    // Es la libreria de Polulu para los sensores LIDAR VL

// Tambien incluimos las librerias de Zumo
#include <ZumoMotors.h> // Consume 2 byte de memoira OK
#include <ZumoBuzzer.h> // Consume 2 bytes de memoria OK
#include <Pushbutton.h> // Consume 8 bytes de memoria OK
#include <QTRSensors.h> // Va con ZumoReflectanteSensorArray
#include <ZumoReflectanceSensorArray.h> // Consume 60 bytes de memoria


#define Parada 0
#define turnSpeed 200     
#define straightSpeed 200 
#define adjustSpeed 150
#define LED 13
#define QTREmiterPin 2  

#define RANGE 1
#define VLIzquierda 0x29     //0010 1001 Address I2C Izquierda
#define VLDerecha 0x30       //0011 0000 Address I2C Derecha
#define VLFrontal 0x46       //0100 0110 Address I2C Frontal 
#define TIzquierda 6         // Shutdown del sensor Izquierda
#define TDerecha 13          // Shutdown del sensor Derecha

// Definimos el giróscopo
L3G gyro; // Esta denominación para el L3G es necesaria por
          // la librería TurnSensor.h.

// Definimos motores
ZumoMotors motors;

// Definimos un altavoz buzzer
ZumoBuzzer buzzer;

// Definimo el pulsador de arranque
Pushbutton button(ZUMO_BUTTON); //Turnsensor utiliza el nombre de button

#include <avr/pgmspace.h>  // Esto nos permite referirnos a datos en el espacion de programa (i.e la flash)
// almacenando esta fuga en el espacio de programa usando la macro PROGMEM.  
// Despues podremos tocar directamente desde el espacio de programa, sin necesidad de cargarlo
// todo en la RAM primero.
const char fugue[] PROGMEM = 
  "! O5 L16 agafaea dac+adaea fa<aa<bac#a dac#adaea f"
  "O6 dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"
  "L8 MS <b-d<b-d MLe-<ge-<g MSc<ac<a ML d<fd<f O5 MS b-gb-g"
  "ML >c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
  "O6 L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"
  "<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"
  "<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
  "O5 e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"
  "e>d>c#>db>d>c#b >c#agaegfe f O6 dc#dfdc#<b c#4";

// Definimos el sensor delantero QTR
// Definimos una clase de sernsor reflectante frontal, sin pin de emisión, no apaga los IR Leds
ZumoReflectanceSensorArray sensorIR(QTREmiterPin); 

// Definimos un array para almacenar los valores del sensor
#define NUM_SENSORES 6
unsigned int ValoresSensor[NUM_SENSORES];
#define NIVEL_DISPARO_QTR  300 // Es el valor de decisión, en microsegundos, tenía anteriormente 300 para borde blanco, 900 cartón marrón borde negro
                               // Hay que pensar que el Dohyo es negro y la línea blanca

// Definimos sensores VL y configuración
VL6180X sensorI; // Izquierda
VL6180X sensorD; // Derecha
VL6180X sensorF; // Frontal


char Camino[250]; //={'L','F','F','L'}; // Camino="LFFL"
uint8_t Long_Camino = 0; //4 // Es la longitud del camino, máximo 250

bool regla=true; // Por defecto es la regla de la mano izquierda
                 // si fuera falso seria la mano derecha

unsigned long tiempo=0; // En nicio para seleccionar regla mano iz o der.
unsigned long IncTiempo=0; // Es para otras posibiliades
int32_t alfa=0; // Giros de 90º. Se pueden hacer otros.
double angulo;  // Para el cálculo de ángulos por la diagonal

// POSICIONAMIENTO DEL ROBOT EN EL TABLERO
// Contador pasos al andar de frente
// Variables volátiles para utilizar en las interrupciones
// en un primer cálculo cada paso anda 0,168111 cm.
// Una celda de 18cm tendría 107 pasos de largo
volatile long contador = 0;
// Es el espacio recorrido total
// Una casilla son 18cm, un giro 0
// Cada vez que se llama a Siguesegmento suma 18
uint16_t ContadorTotal=0; 

int derechoMovimiento = 0;    // En los cambios de velocidad de los motores
int izquierdoMovimiento = 0;  // utlizamos estas variables
boolean luces = LOW;          // Indica luces encendidas o apagadas
uint8_t distancia = 0;        // Es para la distancia frontal
uint8_t N_pasillos = 0; // Contamos el número de pasillos / opiciones en intersecciones
bool pasilloIzquierda=false, pasilloRecto=false, pasilloDerecha=false; // Nos van a indicar si hay pasillos
int Velocidad =-300; // Se puede eliminar cuando eliminemos el control por BT
bool META=false; // Nos va a decir si llegamos a la meta

// Dirección de movimiento
// Por defecto el robot sale en dirección eje x desde (0,0)
int OR_Inicio=0;
int orientacion=OR_Inicio; // Es la direción en la que se mueve el robot partiendo de (0,0)
                           // Puede ser robot en direccion eje x Or_Inicio=0, o eje y Or_Inicio=270
                           // Guardamos Or_inicio para conocer la dirección de origen en segundas
                           // El eje x se incrementa en dirección 0 y decrementa en 270
                           // El eje y se incrementa en dirección 270 y decrementa en 90
// Coordenadas de posición (x,y)
// x e y en el rango 0-15
int8_t x=0,y=0;          

void setup()
{
    // Inicializamos el serial
    Serial.begin(9600);
 
    // Proceso de configuración
    Serial.println(F("\n\n-------------------------------------------"));
    Serial.println(F("Configuración Zumo, versión 8.0, 29/10/2018"));
    Serial.println(F("             Ignacio Otero"));
    Serial.println(F("-------------------------------------------\n\n"));
  
    // Ponemos las interrupciones en el pin 2
    attachInterrupt(0, Pasos, RISING);  // Asociamos la interrupción del pin 2 a la función pasos
    
    // Inicializamos la librería wire
    Wire.begin();
   
       
    // Paramos motores
    motors.setSpeeds(0, 0);
 
    // CONFIGURAMOS SENSORES VL
    Serial.println("Configurando sensores");
    pinMode(TIzquierda,OUTPUT);
    pinMode(TDerecha,OUTPUT);
    digitalWrite(TIzquierda, LOW); // Izquierda
    digitalWrite(TDerecha, LOW); // Derecha

      delay(50);
      sensorF.init();
      sensorF.configureDefault();
      sensorF.setAddress(VLFrontal);
      Serial.print("F ADD =0x");
      Serial.println(sensorF.readReg(0x212),HEX);
      sensorF.writeReg(VL6180X::SYSRANGE__MAX_CONVERGENCE_TIME, 30);
      sensorF.writeReg16Bit(VL6180X::SYSALS__INTEGRATION_PERIOD, 50);
      sensorF.setTimeout(500);
      sensorF.stopContinuous();
      sensorF.setScaling(RANGE);
      delay(300);
      sensorF.startInterleavedContinuous(100);
      delay(100);

      digitalWrite(TDerecha, HIGH);
      delay(200);
      sensorD.init();
      sensorD.configureDefault();
      sensorD.setAddress(VLDerecha);
      Serial.print("D ADD =0x");
      Serial.println(sensorD.readReg(0x212),HEX); // read I2C address
      sensorD.writeReg(VL6180X::SYSRANGE__MAX_CONVERGENCE_TIME, 30);
      sensorD.writeReg16Bit(VL6180X::SYSALS__INTEGRATION_PERIOD, 50);
      sensorD.setTimeout(500);
      sensorD.stopContinuous();
      sensorD.setScaling(RANGE); // configure range or precision 1, 2 oder 3 mm
      delay(300);
      sensorD.startInterleavedContinuous(100);
      delay(100);

      digitalWrite(TIzquierda, HIGH);
      delay(200);
      sensorI.init();
      sensorI.configureDefault();
      //sensorI.setAddress(VLIzquierda); // El último por defecto es el 0x29
      Serial.print("I ADD =0x");
      Serial.println(sensorI.readReg(0x212),HEX);
      sensorI.writeReg(VL6180X::SYSRANGE__MAX_CONVERGENCE_TIME, 30);
      sensorI.writeReg16Bit(VL6180X::SYSALS__INTEGRATION_PERIOD, 50);
      sensorI.setTimeout(500);
      sensorI.stopContinuous();
      sensorI.setScaling(RANGE);
      delay(300);
      sensorI.startInterleavedContinuous(100);
      delay(100); 
 
     Serial.println(F("Sensores LIDAR Listos! \n"));
     delay(100);


    
 //Calibramos el giróscopo
 Serial.println("Calibrando Giroscopo");
 turnSensorSetup();  // Inicializamos el giróscopo y lo calibramos
                    
                     
  // Avisamos que está todo configurado y hay que pulsar botón
  buzzer.playNote(NOTE_G(5), 200, 15);
  Serial.println(F("L3G calibrado y listo. \n"));
 
  turnSensorReset(); // Resetamos los valores iniciales
                                          
   
    pinMode(LED,OUTPUT);
    digitalWrite(QTREmiterPin, LOW);
    
    
    char S ='P';
    for(int cont = 0; cont<100; cont++) 
      {
        if (Serial.available()) S=Serial.read();
        delay(50);
      }

    // Comprobamos estado de la batería
    Serial.print(F("Voltaje por pila = "));
    Serial.print(readBatteryMillivolts()/4);
    Serial.println(F("mV \n"));

    Serial.println(F("Pulsa boton Zumo"));
    buzzer.playNote(NOTE_A(5), 200, 15);
    delay(300);
    buzzer.playNote(NOTE_A(5), 200, 15);
    while (!button.isPressed());  // Espera a que se pulse el botón
    {
        delay(10);         
    } 
    tiempo=millis(); // Tomamos referencia del tiempo
    
    while (button.isPressed());  // Espera a que se libere el botón
    {
        delay(10);
    }

    if((millis()-tiempo)>1000) 
    {
        regla=false; // Pasamos a utilizar la mano derecha
        buzzer.playNote(NOTE_A(5), 200, 15);
        delay(300);
        buzzer.playNote(NOTE_A(5), 200, 15);
        delay(300);
        buzzer.playNote(NOTE_A(5), 200, 15);
        Serial.println(F("Pasamos a la regla de la mano derecha."));
    }
    else Serial.println(F("Por defecto regla de la mano izquierda."));
    
    Serial.println(F("Listos, resolvemos laberinto..."));

    // RESOLVEMOS EL LABERINTO
    ResuelveLaberinto();
}


void loop()
{
// El laberinto está resulto. Esperamos a que se pulse un botón.
  button.waitForButton();

  // Seguimos el camino simplificado calculado antes para alcanzar el final
  // del laberinto.
  SigueCaminoLaberinto();
}

// FUNCION Girar()
// Gira de acuerdo al parámetro Direccion, el cual podra ser 'L'
// (izquierda), 'R' (derecha), 'F' (recto) o 'B' (atrás). Giramos
// usando el giroscopo hasta que sobrepasamos el ángulo correspondiente
// -90º, +90º, 0º o 180º.
void Girar(char Direccion) //turn(char dir)
{
     
    turnSensorReset(); // Inicializamos turnRate y turnAngle
   
    if (Direccion == 'F')
      {
          return;
      }

    switch(Direccion)
       {
         case 'B':
             motors.setSpeeds(turnSpeed, -turnSpeed);
           while((int32_t)turnAngle <= turnAngle45 * 2)
             {
               turnSensorUpdate();
             }
           delay(100);
           
           turnSensorReset();
           motors.setSpeeds(turnSpeed, -turnSpeed);
           while((int32_t)turnAngle <= turnAngle45 * 2)
            {
               turnSensorUpdate();
            } 
           break;

         case 'L':
           motors.setSpeeds(turnSpeed, -turnSpeed);
           while((int32_t)turnAngle <= turnAngle45 * 2)
             {
               turnSensorUpdate();
             }
           break;

         case 'R':
           motors.setSpeeds(-turnSpeed, turnSpeed);
           while((int32_t)turnAngle >= -turnAngle45 * 2)
             {
               turnSensorUpdate();
             }
           break;

     } // Fin del Switch
     
     // Hemos hecho el giro y paramos motores 
     motors.setSpeeds(0, 0);
} // Fin de la función

// FUNCION TomarDecision()
// Esta funcion decide el camino durante la fase de aprendizaje
// en la resolucion del laberinto.  Utiliza las variables found_left,
// found_straight, and found_right, que indican cukndo hay una salida en cada
// una de las tres direcciones, aplicando la estrategia de la regla de la mano izquieda
char TomarDecision(bool pasilloIzquierda, bool pasilloRecto, bool pasilloDerecha, bool regla)
{
    
  if(regla) // Estrategia de la mano izquirda
  {
       if(pasilloIzquierda) { return 'L'; }
       else if(pasilloRecto) { return 'F'; }
       else if(pasilloDerecha) { return 'R'; }
       else { return 'B'; }
  }
  else  // Estrategia de la mano derecha
  {
      if(pasilloDerecha) { return 'R'; }
      else if(pasilloRecto) { return 'F'; }
      else if(pasilloIzquierda) { return 'L'; }
      else { return 'B'; }
  }
}

     
// FUNCION CuentaPasillos()
// Esta función se encarga de comprobar los sensores en los pasillos
// y determinar si la calle está abierta en cada dirección, izquierda
// frente y derecha. Guarda los valores en las variables boolenas correspondientes
// y devuelve un valor con el número de pasillos que encuentra.
uint8_t CuentaPasillos()
{     
    
     N_pasillos = 0; // Inicializamos variables
     pasilloIzquierda=false ;
     pasilloRecto=false ;
     pasilloDerecha=false;
     
     if(sensorI.readRangeContinuousMillimeters()>120)
          {
            pasilloIzquierda=true ;
            N_pasillos++;
          }
     if(sensorF.readRangeContinuousMillimeters()>120)
          {
            pasilloRecto=true ;
            N_pasillos++;
          }
     if(sensorD.readRangeContinuousMillimeters()>120)
          {
            pasilloDerecha=true;
            N_pasillos++;
          }
     return N_pasillos;
}   


// FUNCION ResuelveLaberinto()
// La función ResuelveLaberinto() trabaja simplificando
// la estrategia de la mano izquierda. El robot sigue un segmento
// hasta que alcanza una intersección, donde debe tomar el camino
// más a la izquieda. Guarda cada giro que hace, y si el laberinto
// no tiene bucles, esta estrategia llevará eventualmente al final.
// Despues, el camino recorrido se simplificará eliminando
// las calles muertas.
void ResuelveLaberinto()
{
   Long_Camino = 0;
 
   delay(1000);
  
   buzzer.playFromProgramSpace(PSTR("!L16 cdegreg4"));

  while(1)
  {
    SigueSegmento();
     
    AjustarInterseccion(); //Avanza itersectiondelay

  
                 if(META)
                   {
                     Serial.println("META!");
                     // LLegamos a Meta;
                     break; 
                   } 

    N_pasillos = CuentaPasillos(); 
                   
    char Direccion = TomarDecision(pasilloIzquierda, pasilloRecto, pasilloDerecha, regla);

    if ((N_pasillos > 1)||(Direccion='B'))
        {
             if (Long_Camino >= sizeof(Camino))
                {
                  while(1)
                     {
                        buzzer.playNote(NOTE_A(5), 200, 15);
                        delay(1000);
                     }
                }

             Serial.print(F("Guardamos decision: "));
             Camino[Long_Camino] = Direccion;
             Long_Camino++; // Incrementamos el puntero

             SimplificarCamino();

             MuestraCamino();

             if (Long_Camino == 2 && Camino[0] == 'B' && Camino[1] == 'B')
             {
                  buzzer.playFromProgramSpace(PSTR("!<b4"));
             }
        }
        
    
    Girar(Direccion);
    
    if (Direccion='B')
       {
        motors.setSpeeds(0,0);
        }
    delay(100); // esto lo podremos quitar
   } 

  motors.setSpeeds(0, 0);
  buzzer.playFromProgramSpace(PSTR("!>>a32"));
}

// FUNCION SigueCaminoLaberinto
// El recorrido del laberinto, siguiendo el camino guardado. Necesitamos
// detectar las intersecciones, pero no necesitamos identificar todas
// las salidas de cada una, asi que esta funcion eas más sencilla que 
// ResuelveLaberinto.
void SigueCaminoLaberinto()
{
   buzzer.playFromProgramSpace(PSTR("!>c32"));

   delay(1000);

  uint16_t i = 0;
  while(i<Long_Camino) // La última es o y no es una decisión
    {
  
       SigueSegmento();
       AjustarInterseccion();

       N_pasillos = CuentaPasillos(); 
                 
       char Direccion = TomarDecision(pasilloIzquierda, pasilloRecto, pasilloDerecha, regla);

       if((N_pasillos=0)||(Direccion='B')) // Se supone que nunca habrá una 'B', pero puede equivocarse
         {
          Direccion = Camino[i];
          i++; // Incrementamos i
         }
         Girar(Direccion);
     } 

   motors.setSpeeds(-straightSpeed, -straightSpeed);
  delay(700);
  
  buzzer.playFromProgramSpace(fugue);
  
  delay(1100); 
  motors.setSpeeds(0, 0);
   
 }

// FUNCIÖN SimplificarCamino()
// Simplificacion del camino Camino. La estrategia es que siempre que
// encontremos una secuencia xBx, podramos simplificarla cortando
// el final del tramo. Por ejemplo, LBL -> F, por que una simple F
// hace lo mismo que el final de camino LBL.
void SimplificarCamino()
{
   if(Long_Camino < 3 || Camino[Long_Camino - 2] != 'B')
  {
    return;
  }
  Serial.print(F("Simplificamos: "));
  MuestraCamino();

  int16_t AnguloTotal = 0;

  for(uint8_t i = 1; i <= 3; i++)
  {
    switch(Camino[Long_Camino - i])
    {
    case 'L':
      AnguloTotal += 90;
      break;
    case 'R':
      AnguloTotal -= 90;
      break;
    case 'B':
      AnguloTotal += 180;
      break;
    }
  }

  AnguloTotal = AnguloTotal % 360;
  switch(AnguloTotal)
  {
  case 0:
    Camino[Long_Camino - 3] = 'F';
    break;
  case 90:
    Camino[Long_Camino - 3] = 'L';
    break;
  case 180:
    Camino[Long_Camino - 3] = 'B';
    break;
  case 270:
    Camino[Long_Camino - 3] = 'R';
    break;
  }

  Long_Camino -= 2;
}

// FUNCION MuestraCamino()
void MuestraCamino()
{
  Camino[Long_Camino] = 0;
  Serial.print(F("Longitud = "));
  Serial.print(Long_Camino);
  Serial.print(F(" Camino = "));
  for(uint16_t i=0;i<Long_Camino;i++)
     {
       Serial.print(Camino[i]);
     }
  Serial.println(" ");
 }


// FUNCION SigueSegmento()
// Esta función produce que el robot siga un pasillo hasta que
// detecte una intersección, un muro frontal o la meta.
void SigueSegmento() 
{
             boolean PARED_IZ, PARED_DR;  
             int16_t error;
             uint8_t distancia_I;
             uint8_t distancia_D;
             uint8_t distancia_F;

             distancia_I=sensorI.readRangeContinuousMillimeters();
             distancia_D=sensorD.readRangeContinuousMillimeters();
             if(distancia_I>100) PARED_IZ=false; else PARED_IZ=true;
             if(distancia_D>100) PARED_DR=false; else PARED_DR=true;
             
             while(1) 
               {      
                          distancia_D=sensorD.readRangeContinuousMillimeters();
                          distancia_I=sensorI.readRangeContinuousMillimeters();
                          distancia_F=sensorF.readRangeContinuousMillimeters();
                         
                         sensorIR.read(ValoresSensor); // Leemos el array de sensores
                         if(ValoresSensor[2]<NIVEL_DISPARO_QTR&&ValoresSensor[3]<NIVEL_DISPARO_QTR)
                           {
                             META=true;
                             break; 
                           }

                         if(PARED_IZ) error=distancia_I - 45;  
                         else if(PARED_DR) error=45-distancia_D;
                         else error=0;                          

                         int16_t speedDifference = error * 15; // el error estará entre -45 y + 45, si va recto
                         if (speedDifference > 30) speedDifference = 30;
                         if (speedDifference < -30) speedDifference = -30;

                         int16_t izquierdoMovimiento = (int16_t)straightSpeed + speedDifference;
                         int16_t derechoMovimiento = (int16_t)straightSpeed - speedDifference;

                         izquierdoMovimiento = -constrain(izquierdoMovimiento, 0, 400);//(int16_t)straightSpeed);
                         derechoMovimiento = -constrain(derechoMovimiento, 0,400);//(int16_t)straightSpeed);
                                                
                         motors.setSpeeds(derechoMovimiento, izquierdoMovimiento);
            
                        
                  if (distancia_F<80) 
                   {
                     derechoMovimiento = distancia_F * 4 - 100;
                     izquierdoMovimiento = derechoMovimiento;
                     motors.setSpeeds(-derechoMovimiento, -izquierdoMovimiento);
                     distancia_F=sensorF.readRangeContinuousMillimeters();
                     if(distancia_F<45)
                       {
                       delay(20);
                       break; // Rompemos el bucle while
                       }
                   }  

                  if (PARED_IZ&&(distancia_I>100))
                    {
                     // Deja de haber pared y hay cruce, paramos
                     PARED_IZ=false;
                     break;
                    }
                      
                  if (PARED_DR&&(distancia_D>100))
                   {
                     // Deja de haber pared, hay cruce, paramos
                     PARED_DR=false;
                     break;
                   }
                      
                  if (!PARED_DR&&(distancia_D<100))
                   {

                     PARED_DR=true;
                   }

                  if (!PARED_IZ&&(distancia_I<100))
                   {
                     PARED_IZ=true;
                   }
             }
           
             
             motors.setSpeeds(0, 0);
}   


// FUNCION AjustarInterseccion()
// Esta función debería llamarse despues de followSerment para llevar
// el robot al centro de la intersección.
void AjustarInterseccion()  
{
      motors.setSpeeds(-straightSpeed, -straightSpeed); // Los motores están invertidos
                             
      distancia = sensorF.readRangeContinuousMillimeters();
   
   if (distancia<150)
         {
              while(1)
                 {
                    distancia=sensorF.readRangeContinuousMillimeters();
                    if(distancia<80)
                         {
                            derechoMovimiento = distancia * 4 - 100;
                            izquierdoMovimiento = derechoMovimiento;
                            motors.setSpeeds(-derechoMovimiento, -izquierdoMovimiento);
                            if(distancia<45)
                                  {
                                      motors.setSpeeds(Parada, Parada);
                                      delay(20);
                                      break; // Rompemos el bucle while
                                   }
                          }
                } 
            } 

            else 
                {
                       delay(700);
                       motors.setSpeeds(Parada, Parada);
                }
}


/*  FUNCIÓN LEE BATERÍA
 *  DEVUELVE UN VALOR EN MILIVOLTIOS
 *  APROXIMADO AL VALOR REAL DE LA BATERÍA
 */
inline uint16_t readBatteryMillivolts()
{
    const uint8_t sampleCount = 8;
    uint32_t sum = 0;
    for (uint8_t i = 0; i < sampleCount; i++)
       {
          sum += analogRead(A1);
       }
    sum=sum*15/16-100;
    return ((uint32_t)sum);
}

//******************************************************************
// FUNCION DE INTERRUPCIÓN POR PASOS EN LA SUBIDA PARA MOTOR DERECHO
//******************************************************************
// Esta función cuenta los pasos del encoder por interrupciones
// Cada paso corresponde a 0,168111 cm
void Pasos()
{
  contador++;
}


//******************************************************************
// FUNCION DE AJUSTAR NUEVA CASILLA
//******************************************************************
// Actualiza los valores de x e y en función de contador y la orientación
void ActualizarCasilla()
{
  // x, y, contador y orientación son variables generales
   switch (orientacion) {
                                 case 0:
                                 x++;
                                 break;

                                 case 90:
                                 y--;
                                 break;

                                 case 180:   
                                 x--;
                                 break;

                                 case 270:
                                 y++;
                                 break;
                             } // end switch
                             
                             
                             // No debería pasar pero
                             // comprobamos x e y en el rango de 0-15
                             if(x>15) x=15;
                             if(x<0)  x=0;
                             if(y>15) y=15;
                             if(y<0)  y=0;

                             // Ponemos contador a 0
                             contador=0;
                         
}

