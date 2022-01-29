// КОД ДЛЯ ARDUINO MEGA

#include "DHT.h"

#define DHTPIN 5     
#define DHTTYPE DHT11
#define redLED 2
#define greenLED 3
#define yellowLED 4

int baud = 9600;

// Тестовые данные датчиков.
int Sensors[5];

int temperature;
int humidity;
int pressure;
int moisture;
int water_level;

bool pump_state = false;
bool air_heater_state = false;
bool water_heater_state = false;
bool fan_state = false;
bool moisture_state = false;

DHT dht(DHTPIN, DHTTYPE);

void blinkLED(){
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
}

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
    Serial.print("Incoming byte: ");
    Serial.println(inByte);

    if (inByte == 83){
      // Принятие флага о запросе значении датчиков
      
      temperature = dht.readTemperature();
      humidity = dht.readHumidity();
      pressure = 100;
      moisture = 50;
      water_level = 5;
      
      Sensors[0] = temperature;   
      Sensors[1] = humidity;
      Sensors[2] = pressure;
      Sensors[3] = moisture;
      Sensors[4] = water_level;
      
      // Отправка показании датчиков на ESP32
      Serial1.write((uint8_t*)Sensors, sizeof(Sensors));   // Отправка данных на ESP32 через "Serial Port"      
      break;
    }
    else if (inByte == 84){
      if (pump_state){
          pump_state = false;
        }
        else{
          pump_state = true;
        }
      Serial1.write('A');
      blinkLED();
      break;
    }
    else if (inByte == 76){
      if (air_heater_state){
          air_heater_state = false;
        }
        else{
          air_heater_state = true;
        }
      Serial1.write('A');
      blinkLED();
      break;
    }
    else if (inByte == 70){
      if (water_heater_state){
          water_heater_state = false;
        }
        else{
          water_heater_state = true;
        }
      Serial1.write('A');
      blinkLED();
      break;
    } 
 
 }

    
  // !!! Исполнение управляющих воздействии от ESP32 !!!
  if(pump_state){digitalWrite(yellowLED, HIGH);}else{digitalWrite(yellowLED, LOW);}
  if(air_heater_state){digitalWrite(greenLED, HIGH);}else{digitalWrite(greenLED, LOW);}
  if(water_heater_state){digitalWrite(redLED, HIGH);}else{digitalWrite(redLED, LOW);}
    
    // Обратная связь с ESP32 о принятии и обработке данных управляющих воздействии
}   
  
