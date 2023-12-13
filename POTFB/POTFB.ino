#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h> 

int DS18S20_Pin = 2;
OneWire ds(DS18S20_Pin);  // on digital pin 2

// Registrar la Mac para su proyecto.
// Ingrese la MAC de su shell, para obtenerla, puede acceder a: Archivo->Ejemplos->Ethernet->DhcpAdressPrinter.
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
char serverName[] = "10.58.16.146";    // name address of your domain

// Esta IP se asignará en caso de que el DHCP falle en asignar la ip al servidor
IPAddress ip(192,168,0,177);
int serverPort = 80;
EthernetClient client;
int totalCount = 0;
char pageAdd[64];

char tempString[] = "00.00";

// Setea el delay en milisegundos.
// 5 segundos.
#define delayMillis 5000UL

unsigned long thisMillis = 0;
unsigned long lastMillis = 0;

// Definimos el pin digital donde se conecta el sensor
int pot1 = A0;
int mq2 = A1;

// Inicializamos el sensor DHT11

void setup() {
  Serial.begin(9600);
    //Iniciar Pines
    pinMode(pot1, INPUT);
    pinMode(mq2, INPUT);
  
  // disable SD SPI
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);

  // Iniciar ethernet
  Serial.println(F("Conectando a ethernet..."));
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Falla en la configuración de DHCP");

    Ethernet.begin(mac, ip);
  }

  digitalWrite(10,HIGH);

  Serial.println(Ethernet.localIP());

  delay(2000);
  Serial.println(F("Listo"));
}

void loop()
{  
  thisMillis = millis();

  if(thisMillis - lastMillis > delayMillis)
  {
    lastMillis = thisMillis;

//LLamar método para obtener datos de potenciometro sin mapear
  getPotenciometro1();
  getMQ2();
  }    
}

//Caputra de temperatura C en DHT11
void getPotenciometro1()
  {
   //Lectura en Celsius 
    int val = analogRead(pot1);

    Serial.println("Datos Pot 1:");
    //Al método ftoa, se le envía el arreglo llamado tempString, el valor del sensor, cantidad de decimales
    Serial.println(ftoa(tempString,val,0));
    //Se envian los datos a las API creadas en el servidor, asegurandose de que los datos sean los mismos que recibirá la página php
    //en la variable $arduino_data = $_GET['data_temperatura']; por ejemplo.
    sprintf(pageAdd,"/arduino/Pot.php?arduino_data=%s",ftoa(tempString,val,0));

    if(!getPage(serverName,serverPort,pageAdd)) Serial.print(F("Falla"));
    else Serial.print(F("OK "));
    totalCount++;
    Serial.println(totalCount,DEC);
  }

void getMQ2()
  {
   //Lectura en Celsius 
    int gas = analogRead(mq2);
    gas = map(gas,1000,-2000,0,100);

    Serial.println("Datos MQ2:");
    //Al método ftoa, se le envía el arreglo llamado tempString, el valor del sensor, cantidad de decimales
    Serial.println(ftoa(tempString,gas,0));
    //Se envian los datos a las API creadas en el servidor, asegurandose de que los datos sean los mismos que recibirá la página php
    //en la variable $arduino_data = $_GET['data_temperatura']; por ejemplo.
    sprintf(pageAdd,"/arduino/MQ2.php?arduino_data=%s",ftoa(tempString,gas,0));

    if(!getPage(serverName,serverPort,pageAdd)) Serial.print(F("Falla"));
    else Serial.print(F("OK "));
    totalCount++;
    Serial.println(totalCount,DEC);
  }


//Método que obiene la dirección de la página en la API que permite enviar los datos.
byte getPage(char *ipBuf,int thisPort, char *page)
{
  int inChar;
  char outBuf[128];

  Serial.print(F("Conectando..."));

  if(client.connect(ipBuf,thisPort))
  {
    Serial.println(F("Conectado"));

    sprintf(outBuf,"GET %s HTTP/1.1",page);
    client.println(outBuf);
    sprintf(outBuf,"Host: %s",serverName);
    client.println(outBuf);
    client.println(F("Connection: close\r\n"));
    
  } 
  else
  {
    Serial.println(F("failed"));
    return 0;
  }

  // Este loop controla si el hardware de desconecta
  int connectLoop = 0;

  while(client.connected())
  {
    while(client.available())
    {
      inChar = client.read();
      Serial.write(inChar);
      // Si hay nuevos paquetes, se reinicia el contador
      connectLoop = 0;
    }
    connectLoop++;
    // si el loop tiene más de 10 segundos.
    if(connectLoop > 10000)
    {
      // Entonces cierra la conección por tiempo.
      Serial.println();
      Serial.println(F("Timeout"));
      client.stop();
    } 
    delay(1);
  }
  Serial.println();
  Serial.println(F("Desconectado."));
  client.stop();
  return 1;
}
//Este código permite convertir los valores del sensor a string para su interpretación como valor al ser enviado a la API en PHP.
char *ftoa(char *a, double f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};  
  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
}