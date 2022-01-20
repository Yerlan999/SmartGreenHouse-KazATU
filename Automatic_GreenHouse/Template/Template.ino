#include "DHT.h"

#define DHTPIN 5     
#define DHTTYPE DHT11
#define redLED 2
#define greenLED 3
#define yellowLED 4

float temperature;
float humidity;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  
  pinMode(redLED,OUTPUT);
  pinMode(greenLED,OUTPUT);
  pinMode(yellowLED,OUTPUT);
  
  dht.begin();
}

void loop() {
  delay(1000);  
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  Serial.println(humidity);
  Serial.println(temperature);
  Serial.println("");
  }
