//5MA GRUPO 2 RIVAS FRACCARO GUZMAN
#include <U8g2lib.h>
#include "DHT.h"
#include <Preferences.h>
Preferences preferences;


U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


#define DHTPIN 23
#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE);


#define SW1 34
#define SW2 35
#define P1 80
#define ESPERA1 82
#define P2 81
#define ESPERA2 83
#define ESPERA3 84
int estadoActual = P1;
int tiempo;
float umbral = 28;
float temp;
char stringtemp[10];
char stringumbral[10];


void setup()
{
  Serial.begin(9600);
  pinMode (SW1, INPUT_PULLUP);
  pinMode (SW2, INPUT_PULLUP);
  Serial.println(F("OLED test"));
  Serial.println(F("DHTxx test!"));
  u8g2.begin();
  dht.begin();
  preferences.begin("prietto", true); // Modo solo lectura
  if (preferences.isKey("umbral")) {
    umbral = preferences.getFloat("umbral", 28); // Si no hay, usa 28 por defecto
  }
  preferences.end();
}


void loop()
{
  temp = dht.readTemperature();
  if (isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  switch (estadoActual){
      case P1:
      u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font
      u8g2.clearBuffer();
      sprintf (stringtemp, "%.2f" , temp); ///convierto el valor float a string
      u8g2.drawStr(60, 25, stringtemp); // write something to the internal memory
      sprintf (stringumbral, "%.2f" , umbral);
      u8g2.drawStr(60, 50, stringumbral); // write something to the internal memory
      u8g2.drawStr(25, 50, "VU:"); // write something to the internal memory
      u8g2.drawStr(25, 25, "VA:"); // write something to the internal memory
      u8g2.sendBuffer();
      if (digitalRead(SW1) == LOW){
        tiempo = millis();
        estadoActual = ESPERA1;
      }
      break;
    case ESPERA1:
      if (digitalRead(SW1)==HIGH){
        if (millis() - tiempo >= 5000){
          estadoActual = P2;
        }
        else{
          estadoActual = P1;
        }
      }
      break;
    case P2:
      u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font
      u8g2.clearBuffer();
      sprintf (stringumbral, "%.2f" , umbral);
      u8g2.drawStr(60, 37, stringumbral); // write something to the internal memory
      u8g2.drawStr(25, 37, "VU:"); // write something to the internal memory
      u8g2.sendBuffer();
      if (digitalRead (SW1)==LOW){
        estadoActual = ESPERA2;
      }
      if (digitalRead (SW2)==LOW){
        tiempo = millis();
        estadoActual = ESPERA3;
      }
      break;
    case ESPERA2:
      if (digitalRead (SW1) == HIGH){
        umbral = umbral +1;
        estadoActual = P2;
      }
      break;
    case ESPERA3:
      if (digitalRead(SW2)==HIGH){
        if (millis() - tiempo >= 5000){
          preferences.begin("prietto", false); // Modo escritura
          preferences.putFloat("umbral", umbral);
          preferences.end();
          estadoActual = P1;
        }
        if (millis() - tiempo <= 5000){
          umbral = umbral-1;
          estadoActual = P2;
        }
      }
      break;
  }
}
