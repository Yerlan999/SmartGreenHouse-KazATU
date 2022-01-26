// КОД ДЛЯ ARDUINO MEGA

#include "DHT.h"

#define DHTPIN 5     
#define DHTTYPE DHT11
#define redLED 2
#define greenLED 3
#define yellowLED 4

int baud = 9600;
int pointer = 0;

// Тестовые данные датчиков.
int Sensors[5];
int Actuators[20];

int temperature;
int humidity;
int pressure;
int moisture;
int water_level;

int pump_state = 0;
int air_heater_state = 0;
int water_heater_state = 0;
int fan_state = 0;
int moisture_state = 0;

bool actuatorsRecieved = false;
bool endRecieve = false;
    
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  pinMode(redLED,OUTPUT);
  pinMode(greenLED,OUTPUT);
  pinMode(yellowLED,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(baud);                // Для вывода на монитор
  Serial1.begin(baud);   // Прием/Отправка на ESP32
  
  dht.begin();
}


void loop() {
   
  while (Serial1.available() > 0){
    
    int inByte = Serial1.read();

    if (inByte == 65){
      // Принятие флага о запросе значении датчиков
      
      actuatorsRecieved = false;
      
      temperature = dht.readTemperature();
      humidity = dht.readHumidity();
      pressure = 100;
      moisture = 50;
      water_level = 5;
      
      Serial.println(temperature);
      
      Sensors[0] = temperature;   
      Sensors[1] = humidity;
      Sensors[2] = pressure;
      Sensors[3] = moisture;
      Sensors[4] = water_level;
      
      // Отправка показании датчиков на ESP32
      Serial1.write((uint8_t*)Sensors, sizeof(Sensors));   // Отправка данных на ESP32 через "Serial Port"      

    }
    else{
      actuatorsRecieved = true;
      Actuators[pointer] = inByte;
//      Serial.print("Passed: ");         // ХЗ почему, но без данного вывода на экран диоды не горят
//      Serial.println(pointer);      
      pointer += 1;
    }
  }
  if (pointer > 18){
    endRecieve = true;
    pointer = 0;
  }

  
  if (actuatorsRecieved && endRecieve){
    pump_state = Actuators[0];
    air_heater_state = Actuators[4];
    water_heater_state = Actuators[8];
    fan_state = Actuators[12];
    moisture_state = Actuators[16];
    
    actuatorsRecieved = false;
    endRecieve = false;

    
    // Обратная связь с ESP32 о принятии и обработке данных управляющих воздействии
  }   
  
  // !!! Исполнение управляющих воздействии от ESP32 !!!
  if(pump_state){digitalWrite(yellowLED, HIGH);}else{digitalWrite(yellowLED, LOW);}
  
  if(air_heater_state){digitalWrite(greenLED, HIGH);}else{digitalWrite(greenLED, LOW);}

  if(water_heater_state){digitalWrite(redLED, HIGH);}else{digitalWrite(redLED, LOW);}
}
