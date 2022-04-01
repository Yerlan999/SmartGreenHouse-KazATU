// КОД ДЛЯ ARDUINO MEGA

#include "DHT.h"

#define DHTPIN 5     
#define DHTTYPE DHT11
#define redLED 2
#define greenLED 3
#define yellowLED 4

int baud = 9600;

int temperature;
int humidity;
int light;
int moisture;
int water_temperature;

bool pump_state = false;
bool air_heater_state = false;
bool water_heater_state = false;
bool fan_state = false;
bool moisture_state = false;

DHT dht(DHTPIN, DHTTYPE);


void sendFeedBack(){
  Serial1.write(1);
  
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

  // Когда получаю сигнал от ESP32...
  while (Serial1.available() > 0){
    
    int inByte = Serial1.read();
    Serial.print("Incoming byte: ");
    Serial.println(inByte);

    if (inByte == 83){ // "S" == 083 в ASCII 
      // Принятие флага о запросе значении датчиков
      
      temperature = dht.readTemperature();
      humidity = dht.readHumidity();
      light = 100;
      moisture = 50;
      water_temperature = 5;
      
      // Отправка показании датчиков на ESP32
      Serial1.write(temperature);
      Serial1.write(humidity);
      Serial1.write(light);

      // Сверка отправленных данных на ESP32
      Serial.println();
      Serial.println("Sent values: ");
      Serial.println();
      Serial.print("temperature: ");
      Serial.println(temperature);
      Serial.print("humidity: ");
      Serial.println(humidity);    
      Serial.print("light: ");
      Serial.println(light);

      
    }

    // Принятие флага о ВКЛ/ВЫКЛ реле
    
    else if (inByte == 84){ // "T" == 083 в ASCII
      if (pump_state){
          pump_state = false;
        }
        else{
          pump_state = true;
        }
      sendFeedBack();
      break;
    }
    else if (inByte == 76){ // "L" == 083 в ASCII
      if (air_heater_state){
          air_heater_state = false;
        }
        else{
          air_heater_state = true;
        }
      sendFeedBack();
      break;
    }
    else if (inByte == 70){ // "F" == 083 в ASCII
      if (water_heater_state){
          water_heater_state = false;
        }
        else{
          water_heater_state = true;
        }
      sendFeedBack();
      break;
    } 
 
 }

    
  // !!! Исполнение управляющих воздействии от ESP32 !!!
  if(pump_state){digitalWrite(yellowLED, HIGH);}else{digitalWrite(yellowLED, LOW);}
  if(air_heater_state){digitalWrite(greenLED, HIGH);}else{digitalWrite(greenLED, LOW);}
  if(water_heater_state){digitalWrite(redLED, HIGH);}else{digitalWrite(redLED, LOW);}
   
}   
  
