#include <Arduino.h>
#include <WiFi.h>       //for wifi IoT
#include "DHT.h"        //library for DHT
#include "ThingSpeak.h" //for connection with thingspeak (IoT Platform on cloud.)

#define DHTPIN 2      // sensor connected on GPIO2 of module
#define DHTTYPE DHT11 // sensor type DHT11
DHT dht(DHTPIN, DHTTYPE);

const int motorPin = 9;
const int moisutrePin = A0;


// ssid and password for module for conneting with cloud.
const char *ssid = "Neej";
const char *password = "1234567890";
WiFiClient client;

unsigned long myChannelNumber = 12735735;  // channel name of my thingspeak account for data monitioring.
const char *myWriteAPIKey = "3xxxxxxxxxB"; // write api key for my channel.

// constant time delays
unsigned long lastTime = 0;
unsigned long timeDelay = 1500;

// constant values
float temperatureC;
float humidity;

// for setup the module and platform.
void setup()
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  dht.begin();
  pinMode(motorPin, OUTPUT); //pin mode of motor
  digitalWrite(motorPin, LOW); //initial pin mode low
}

void loop()
{
  // connecting the module with internet.
  if ((millis() - lastTime) > timeDelay)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.print("Attempting to Connect!");
      while (WiFi.status() != WL_CONNECTED)
      {
        WiFi.begin(ssid, password);
        delay(4000);
      }
      Serial.println("Connected.");
    }
  }

  delay(2000);                          // delay before starting.
  humidity = dht.readHumidity();        // for humidity.
  temperatureC = dht.readTemperature(); // for temperature in celcuis
  int moisturePrecentage= (100.00 -((analogRead(moisutrePin)/4096.00)*100.00)); //4096 because it have 12 bit ADC if it have 10 bit then it will be 1024.
  int motorstatus=0;
  if (isnan(humidity) || isnan(temperatureC) || isnan(moisturePrecentage))
  { // isnan means is not a number.
    Serial.println("Failed to Fetch the data.");
    return;
  }
  else
  {
    ThingSpeak.setField(1, humidity);                               // 1 shows the graph 1
    ThingSpeak.setField(2, temperatureC);                           // 2 shows the graph 2
    ThingSpeak.setField(3, moisturePrecentage); // 3 shows the graph 3
    if (moisturePrecentage==0){
      motorstatus=1;
      digitalWrite(motorPin, HIGH);
    }
    else{
      motorstatus=0;
      digitalWrite(motorPin, LOW);
    }
    ThingSpeak.setField(4, motorstatus); //4 shows the graph 4.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); // adding the data in thingspeak plantform on my account.
    if (x == 200)
    {
      Serial.println("Channel Updated Sucessfully.");
    }
    else
    {
      Serial.println("Problem in updating the channel. HTTP Error Found " + String(x));
    }
    lastTime = millis();
  }
}
