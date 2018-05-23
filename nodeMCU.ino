#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h" 
#include <SimpleTimer.h>   

#define DHTTYPE DHT11      
#define dht_dpin 2 //d4

WiFiClient client;
DHT dht(dht_dpin, DHTTYPE); 

const int Ana = A0;  
const int SYSTEM = 16;
const int flagH = 14; //d5
const int flagT = 12; //d6
const int flagL = 13; //d7

//thingsspeak
String apiKey = "6OHMA0EUQE4X9Q5Q"; 
const char* server = "api.thingspeak.com";
//BLINK
char auth[] = "9bdc6e110d124a8086b2fa2d5df745a3";
//WIFI
char ssid[] = "AndroidAP-WY";
char pass[] = "sdfw2814";


SimpleTimer timerB;
SimpleTimer timerTS;
float t;                                   // Declare the variables 
float h;
float l;

void setup()
{
    Serial.begin(115200); 
    Blynk.begin(auth, ssid, pass);
    dht.begin();
    timerB.setInterval(1000, sendUptime);
    timerTS.setInterval(20000, sendDataTS);
}

void sendUptime()
{
  if(digitalRead(SYSTEM)){
    
      if(digitalRead(flagH)){
        h = dht.readHumidity();
        Serial.print("humidity = ");
        Serial.println(h);
        Blynk.virtualWrite(V0, h);
      }else{
          Serial.println("humidity off");
      }
      if(digitalRead(flagT)){ 
        t = dht.readTemperature(); 
        Serial.print("temperature = ");
        Serial.println(t); 
        Blynk.virtualWrite(V1, t);    
      }else{
          Serial.println("temperature off");
      }
      if(digitalRead(flagL)){ 
        l = analogRead(Ana);
        Serial.print("luminosity = ");
        Serial.println(l); 
        Blynk.virtualWrite(V2, l);
      }else{
          Serial.println("luminosity off");
      }
      
  }else{
    Serial.println("system off");
  }
  Serial.println();
}

void sendDataTS(){
  if(digitalRead(SYSTEM)){
  
        if (isnan(h) || isnan(t) || isnan(l) ) 
         {
             Serial.println("\r\nFailed to read from sensores!\r\n\r\n");
              return;
         }
         if(digitalRead(flagH) || digitalRead(flagT) || digitalRead(flagL)){
           if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
            {  
                String postStr = apiKey;
               
                if(digitalRead(flagH) && !isnan(h)){ 
                   postStr +="&field1="; 
                   postStr += String(h); 
                }
                if(digitalRead(flagT) && !isnan(t)){ 
                   postStr +="&field2="; 
                   postStr += String(t); 
                }
                if(digitalRead(flagL) && !isnan(l)){ 
                   postStr +="&field3="; 
                   postStr += String(l); 
                }
                
               postStr += "\r\n\r\n";
    
               client.print("POST /update HTTP/1.1\n");
               client.print("Host: api.thingspeak.com\n");
               client.print("Connection: close\n");
               client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
               client.print("Content-Type: application/x-www-form-urlencoded\n");
               client.print("Content-Length: ");
               client.print(postStr.length());
               client.print("\n\n");
               client.print(postStr);
                         
               Serial.println("\r\nDATA SENDED:" + postStr);
          }
          client.stop();
        }else{
            Serial.print("\r\nsensores off!\r\n\r\n");
        }
  } 
}


void loop()
{
  Blynk.run();
  timerB.run();
  timerTS.run();
}
