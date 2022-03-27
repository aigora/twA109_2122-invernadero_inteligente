# Invernadero inteligente

Nuestra propuesta consiste en facilitar el cuidado óptimo de jardines, huertos y zonas
verdes, además de cultivos de interés comercial.

## Integrantes del equipo

Patricio Juan Peñalver Carvajal @patriciopenalver

Luis Vicente Gordón @luisvicentegordon

Carolina Plaza Reynaldo

## Nuestros objetivos son:

● En condiciones de baja iluminación los sensores de luminosidad lo detectarán y tras
recibir la información se ejecutará una orden de encender luces led dispuestas a lo
largo del invernadero.
○ Esto ayudará a las plantaciones a tener un crecimiento de mayor rapidez y
eficacia

● Se dispondrá de un sensor de humedad en suelo el cual estará conectado a un
riego automático para que se active cuando haya una disminución de esta.

● Gracias a un sensor de temperatura la controlaremos en el interior del invernadero y
la regulamos mediante un flujo de aire generado por dos ventiladores dispuestos en
el techo del invernadero.

## Componentes necesarios para la realización de este proyecto:

● Módulo inalámbrico AZDelivery ESP32 ESP-WROOM-32

● Sensor de temperatura: Analog devices Inc. TMP37FT9Z

● Sensor de humedad: módulo YL-69 Sensor y módulo HC-38 para adaptarlo al esp32

● Sensor de luminosidad: GUVA-S12SD series Luz

● Tira led

● Ventiladores

● Bomba de agua

● Relés

## Sensores :
Sensor de Humedad de suelo (Módulo YL-69 Sensor)
Este sensor tiene la capacidad de medir la humedad del suelo. Aplicando una pequeña tensión entre los terminales del módulo YL-69 hace pasar una corriente que depende básicamente de la resistencia que se genera en el suelo y ésta depende mucho de la humedad. Por lo tanto, al aumentar la humedad la corriente crece y al bajar la corriente disminuye.
Consiste en una sonda YL-69 con dos terminales separados adecuadamente y un módulo YL-38 que contiene un circuito, un led de encendido y otro de activación de salida digital. 

módulo HC-38 
Adaptador para Arduino


Sensor de Humedad en Suelo conectado al riego

/*POSIBLES CASOS

1 Nivel bajo de agua. Muestra humedad del suelo. Da aviso rellenar tanque
2 Nivel agua OK. Aviso nivel OK
      2.1 Humedad suelo OK. Muestra humedad suelo
      2.2 Humedad suelo NO OK. Muestra humedad suelo. Comienza riego.
*/


//Configuro pines y variables

const int nivel = 9; //Pin 9 para medir el nivel de agua
const int bomba = 13; //Pin 13 para la bomba
const int humedadsuelo = A0; //A0 para la humedad del suelo


///////////////////////////////parte de humedad suelo y riego

void setup()
{
  Serial.begin(9600);//Arrancamos el puerto serie a 9600
  
  //Configuramos entradas y salidas
  pinMode(humedadsuelo, INPUT);//Configuro humedadsuelo como entrada
  pinMode(bomba, OUTPUT);//Configuro bomba como salida
  pinMode(nivel, INPUT);//Configuro en nivel de agua como entrada
 }

void loop()
{
  int SensorValue = analogRead(humedadsuelo);//Leo el valor de la humedad y lo meto en SensorValue
  int SensorNivel = digitalRead(nivel); //Leo lo que marca el nivel de agua
  
  
  //Imprimo por el puerto serie el valor de la humedad del suelo
  Serial.print("Humedad del suelo: ");Serial.print(SensorValue); Serial.println("%");
  delay(3000);
  
if (SensorNivel==0)
  { Serial.println("Nivel bajo de Agua. Rellenar el tanque"); 
  delay(2000);}

if (SensorNivel==1) 
  {
   Serial.println("Nivel de agua correcto, se puede regar");
   if(SensorValue >= 700) // el valor que considero seco y hay que regar es de 700
  {
   // Si la tierra está seca, comenzara a regar
   // Riega durante 1 segundo y espera a comprobar la humedad otro segundo
   Serial.println("La tierra está seca, comienza el riego");
   digitalWrite(bomba, HIGH);
   delay(2000);
   digitalWrite(bomba, LOW);
   delay(1000);
  }
   }  
  
  //Si la tierra no está seca, no riega y da el valor de la humedad
  delay(1000);
}


Sensor de temperatura y humedad ambiente DHT11
El sensor de temperatura y humedad digital DHT11 es un sensor compuesto que contiene una señal de salida calibrada de temperatura y humedad con un alto grado de confiabilidad y una excelente estabilidad a largo plazo.
Además es una herramienta valiosa debido a su amplio abanico de aplicaciones. Este sensor es ampliamente usado en el control de riego y cultivo de explotaciones agrícolas y de jardinería. Estas medidas ayudarán a las personas al cargo en la toma correcta de decisiones.

Sensor de temperatura y humedad conectado a un sistema de ventilación

#include <DHT.h>

int sensor = 2;
int ventilador = 13;
int temp, humedad;

DHT dht (sensor, DHT11);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(ventilador, OUTPUT);
}

void loop() {

 humedad = dht.readHumidity();
 temp = dht.readTemperature();
 Serial.print("Temperatura: ");
 Serial.print(temp);
 Serial.print("ºC Humedad: ");
 Serial.print(humedad);
 Serial.println("%");
 delay(1000);
 
 if (temp >= 23) { digitalWrite (ventilador, HIGH);
 Serial.print("Ventilador activo. Temperatura: ");
 Serial.print(temp);
 Serial.print(" ºC ");
 delay(5000);}
 
 else { digitalWrite (ventilador, LOW);}
  }

Sensor de luz BH1750
Un sensor de luminosidad es un dispositivo que permite conocer el grado de iluminación de un entorno concreto. Los sensores de luminosidad suelen estar conectados a otros dispositivos, que son los receptores de esta información respecto a la luminosidad ambiental detectada y, a partir de esta información, actúan de un modo u otro.
 
Tiene muchas aplicaciones pero, de entre todas ellas, la más destacable de todas es que nos permite hacer un uso mucho más eficiente de la energía que utilizamos, adaptando la potencia de las luminarias a la luz ambiental existente.

Sensor de Luz conectado a un led


int led = 13;
int lecturasensor;
void setup() {
  serial.begin(9600);
  pinMode(led, OUTPUT);
}
void loop() {
 lecturasensor= analogRead(A0);
 Serial.println(lecturasensor);
 if(lecturasensor>600){
  digitalWrite(led, HIGH);
 }
 else{
  digitalWrite(led, LOW);
 }
}

