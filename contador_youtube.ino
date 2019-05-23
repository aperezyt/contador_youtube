/**
 * Programa para la obtención de seguidores en tiempo real de un canal de Youtube utilizando una placa ESP32 y una matriz de leds con el controlador MAX7219
 * Programa codificado por Alejandro Perez
 */
#include <SPI.h>
#include <YoutubeApi.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h> 
#include "LedMatrix.h"

#define NUMBER_OF_DEVICES 4 //numero de matrices LEDS
#define CS_PIN 15
#define CLK_PIN 14
#define MISO_PIN 2 //we do not use this pin just fill to match constructor
#define MOSI_PIN 12

//Youtube
#define API_KEY "PONER TU API KEY para YOUTUBE" //API Key para consultas de Youtube
#define CHANNEL_ID "UCkdFXlSdL1cmYbpbry2wsdA" // ID del canal

//Acceso a la red (Solo 2.4Ghz)
const char* ssid = "NOMBRE DE LA RED"; //Nombre de la red WiFi
const char* password = "PASSWORD"; //Password

//Objeto para el control de la matriz de LED (SPI)
LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CLK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

//Youtube
WiFiClientSecure client;
YoutubeApi api(API_KEY, client);
long subs = -1;
long previousSubs = 0;
int i = 0;
  
void setup() {
  Serial.begin(9600); 
  delay(10);
  //Establecemos la conexion con nuestra red WiFi
  WiFi.begin(ssid, password); 
  Serial.print("\nConectando a ");
  Serial.print(ssid);
  Serial.print("\n");
  ledMatrix.setIntensity(14); // range is 0-15
  ledMatrix.setCharWidth(7);
  ledMatrix.init();
  ledMatrix.setText("WiFi");
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect     
      ledMatrix.clear();
      ledMatrix.scrollTextLeft();
      ledMatrix.drawText();
      ledMatrix.commit();
      delay(100);
  } 
  Serial.println("Conexion establecida");  
  Serial.print("La direccion IP local es: ");
  Serial.println(WiFi.localIP()); 
 
}

void loop() { 
  if (i % 100 == 0){
      youtubeStats();
  }  
  ledMatrix.clear();
  ledMatrix.scrollTextLeft();
  ledMatrix.drawText();
  ledMatrix.commit();
  i++;
  delay(100);  
}

void youtubeStats(){
  previousSubs = subs;
  api.getChannelStatistics(CHANNEL_ID);
  Serial.print("Subscriber Count: ");
  subs = api.channelStats.subscriberCount;
  Serial.println(subs);
  if (previousSubs != subs && previousSubs != -1){ 
    int dif = subs - previousSubs;
    if(dif<0)
      ledMatrix.setText((String) dif);
    else
      ledMatrix.setText("+"+(String) dif);
    ledMatrix.setNextText((String) subs);
  }else if(previousSubs == -1){
      ledMatrix.setText((String) subs);
  }  
}
