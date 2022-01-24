#include "DHT.h"

#define DHTPIN 5     
#define DHTTYPE DHT11

int values[3];

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  dht.begin();
  delay(500);
}

void loop() {
  
  while (Serial1.available()>0){
    
    if (Serial1.read() == 65){
      
      values[0] = dht.readHumidity();
      values[1] = dht.readTemperature();
      values[2] = 0;
      
      Serial1.write((uint8_t*)values, sizeof(values)); 
       
    }   
  }
}
