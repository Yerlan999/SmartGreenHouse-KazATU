// КОД ДЛЯ ARDUINO MEGA

#include "DHT.h"

#define DHTPIN 5     
#define DHTTYPE DHT11
#define redLED 2
#define greenLED 3
#define yellowLED 4

int baud = 115200;
int pointer = 0;

const unsigned int maxSize = 5;

byte from_ESP32;          // Для хранения СПИСКА со значениями актуаторов от ESP32

// Тестовые данные датчиков.
int Sensors[5];
int Actuators[5];          // Для хранения значении актуаторов с РЕАЛЬНЫМ типом данных

int temperature;
int humidity;
int pressure;
int moisture;
int water_level;
    
DHT dht(DHTPIN, DHTTYPE);

void getSensorReadings(){
    
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    pressure = 100;
    moisture = 50;
    water_level = 5;
    
    delay(2000);
    
    Sensors[0] = temperature;   
    Sensors[1] = humidity;
    Sensors[2] = pressure;
    Sensors[3] = moisture;
    Sensors[4] = water_level;
}

void setup() {
  pinMode(redLED,OUTPUT);
  pinMode(greenLED,OUTPUT);
  pinMode(yellowLED,OUTPUT);

  Serial.begin(baud, SERIAL_8N1);                // Для вывода на монитор
  Serial1.begin(baud, SERIAL_8N1);   // Прием/Отправка на ESP32
  
  dht.begin();
}


void sendToESP32(){
     
  getSensorReadings();      // Получение значении датчиков 
  Serial1.write((uint8_t*)Sensors, sizeof(Sensors));   // Отправка данных на ESP32 через "Serial Port"      

}

void loop() {
  
  sendToESP32();
  
  while (Serial1.available() > 0){
  
    from_ESP32 = Serial1.read();
    Serial.println(from_ESP32);
    
    
    if (pointer < maxSize){
      Serial.println(from_ESP32);
      
      // HANDING INCOMING ACTUATORS VALUE
      Actuators[pointer] = from_ESP32;
      pointer += 1;
    }
  }
  if (pointer != 0){
    Serial1.write('E');
  }
  pointer = 0; 
}
