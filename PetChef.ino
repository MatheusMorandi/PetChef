//Bibliotecas que iremos utilizar

#include <ESP8266WiFi.h>

#include "Adafruit_MQTT.h"

#include "Adafruit_MQTT_Client.h"

#include <Servo.h>

#include <NTPClient.h>

#include <WiFiUdp.h>

// Setup Wifi

#define WIFI_SSID "***********" //Nome da sua rede Wifi

#define WIFI_PASS "***********" //Senha da sua rede wifi

#define MQTT_SERV "io.adafruit.com"

#define MQTT_PORT 1883

#define MQTT_NAME "***********" //Seu nome de usuário no Adafruit

#define MQTT_PASS "***********" //Sua chave do adafruit

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org", -10800, 60000); //Configuração de horário para o Brasil

Servo servo;

int PINO_SERVO = D3;    // Porta em que o motor esta posicionado

int ANG_ABRE = 120;

int ANG_FECHA = 0;

int  hora, mn, seg;

int hora_alim = 0;

int min_alim = 0;

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe petchef = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/petchef");

boolean feed = true; 

void setup()
{
  Serial.begin(74880);
  timeClient.begin();
  
  //Conectando ao wifi
  Serial.print("\n\nConectando ao Wifi... ");

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  Serial.println("Conectado!");

  mqtt.subscribe(&petchef);

  servo.attach(PINO_SERVO);

  servo.write(ANG_FECHA);
  
}

void loop()
{

  MQTT_connect();

  timeClient.update();

  hora = timeClient.getHours();

  mn = timeClient.getMinutes();
   
  seg = timeClient.getSeconds();

  Serial.print("Hora: ");

  Serial.print(hora);

  Serial.print(":");

  Serial.print(mn);

  Serial.print(":");

  Serial.println(seg);
    
  Serial.print("Hora de Alimentar: ");

  Serial.print(hora_alim);

  Serial.print(":");

  Serial.println(min_alim);

  delay(1000);

  Adafruit_MQTT_Subscribe *subscription;

  while ((subscription = mqtt.readSubscription(1000)))

  {
    
    if (subscription == &petchef)

    {
      
      Serial.println((char*) petchef.lastread);
     
    if (!strcmp((char*) petchef.lastread, "ON"))

      {

        servo.write(ANG_ABRE);

        delay(1000);

        servo.write(ANG_FECHA);

      }

      if (!strcmp((char*) petchef.lastread, "TARDE"))

      {

        hora_alim = 18;

        min_alim = 30; 

      }

    }

  }

  if( hora == hora_alim && mn == min_alim &&feed==true) 

    { 

      servo.write(ANG_ABRE);

      delay(1000);

      servo.write(ANG_FECHA);

      feed = false; 

    } 
}
  
void MQTT_connect()
{

  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Conectando ao MQTT... ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));

    Serial.println("Tentando novamente conectar ao MQTT em 5 segundos...");

    mqtt.disconnect();

    delay(5000); 

    retries--;

    if (retries == 0) {

      while (1);

    }

  }

  Serial.println("MQTT Connected!"); 

}
