// КОД ДЛЯ ARDUINO MEGA

#include "DHT.h"
#include <FastLED.h>

#define DHTPIN 5     
#define DHTTYPE DHT11

#define LED_PIN     9
#define NUM_LEDS    25
#define LED_BRIGHTNESS 5 // [0 - 255]

// Блок констант для реле 
#define relay1 23
#define relay2 25
#define relay3 27
#define relay4 29
#define relay5 31
#define relay6 33
#define relay7 35
#define relay8 37

#define tank_fill_relay 39
#define first_floor_relay 41
#define second_floor_relay 43
#define third_floor_relay 45

int baud = 9600;

// Переменные для хранения значении с датчиков
int temperature;
int humidity;
int light;
int carbon;
int water_temperature;
int water_level;
int water_flow;

bool pump_state = false;
bool air_heater_state = false;
bool air_humiditer_state = false;
bool water_heater_state = false;
bool fan_state = false;
bool outlet_fan_state = false;
bool phytolamp_state = false;
bool water_tank_filler_state = false;

bool tank_fill_relay_state = false;
bool first_floor_relay_state = false;
bool second_floor_relay_state = false;
bool third_floor_relay_state = false;

DHT dht(DHTPIN, DHTTYPE);
CRGB leds[NUM_LEDS];

// Функция для отправки обратной связи на ESP32
void sendFeedBack(){
  Serial1.write(1);
  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
    
}


void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(relay3,OUTPUT);
  pinMode(relay4,OUTPUT);
  pinMode(relay5,OUTPUT);
  pinMode(relay6,OUTPUT);
  pinMode(relay7,OUTPUT);
  pinMode(relay8,OUTPUT);

  pinMode(tank_fill_relay,OUTPUT);
  pinMode(first_floor_relay,OUTPUT);
  pinMode(second_floor_relay,OUTPUT);
  pinMode(third_floor_relay,OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(baud);    // Для вывода на монитор
  Serial1.begin(baud);   // Прием/Отправка на ESP32
  
  dht.begin();
}


void loop() {

  // Когда получаю сигнал от ESP32...
  while (Serial1.available() > 0){
    
    int inByte = Serial1.read();
//    Serial.print("Incoming byte: ");
//    Serial.println(inByte);

    if (inByte == 83){ // "S" == 083 в ASCII 
      // Принятие флага о запросе значении датчиков

      // ЗАПРОС ЗНАЧЕНИИ ОТ ДАТЧИКОВ
      temperature = dht.readTemperature();
      humidity = dht.readHumidity();
      light = 100;
      carbon = 5;
      water_temperature = 5;
      water_level = 5;
      water_flow = 5;

      // Отправка показании датчиков на ESP32
      Serial1.write(temperature);
      Serial1.write(humidity);
      Serial1.write(light);
//      Serial1.write(carbon);
//      Serial1.write(water_temperature);
//      Serial1.write(water_level);
//      Serial1.write(water_flow);
      
//      // Сверка отправленных данных на ESP32
//      Serial.println();
//      Serial.println("Sent values: ");
//      Serial.println();
//      Serial.print("temperature: ");
//      Serial.println(temperature);
//      Serial.print("humidity: ");
//      Serial.println(humidity);    
//      Serial.print("light: ");
//      Serial.println(light);

      
    }

    else if (inByte == 65){ // "А" == 065 в ASCII 
      // Принятие флага о запросе значении исполнительных механизмов
      
      // Отправка показании датчиков на ESP32
      Serial1.write(pump_state);
      Serial1.write(air_heater_state);
      Serial1.write(air_humiditer_state);      
      Serial1.write(water_heater_state);
      Serial1.write(fan_state);
      Serial1.write(outlet_fan_state);
      Serial1.write(phytolamp_state);
      Serial1.write(water_tank_filler_state);
      
      
    // Принятие флага о ВКЛ/ВЫКЛ реле
    
    else if (inByte == 84){ // "T" == 084 в ASCII
      if (air_heater_state){
          air_heater_state = false;
        }
      else{
          air_heater_state = true;
        }
      sendFeedBack();
      break;
    }
    else if (inByte == 97){ // "a" == 097 в ASCII
      air_heater_state = true;
      sendFeedBack();
      break;
    }
    else if (inByte == 98){ // "b" == 098 в ASCII
      air_heater_state = false;
      sendFeedBack();
      break;
    }
    
    
    
    
    else if (inByte == 76){ // "L" == 076 в ASCII
      if (phytolamp_state){
          phytolamp_state = false;
        }
      else{
          phytolamp_state = true;
        }
      sendFeedBack();
      break;
    }
    else if (inByte == 99){ // "c" == 099 в ASCII
      phytolamp_state = false;
      sendFeedBack();
      break;  
    }
    else if (inByte == 100){ // "d" == 100 в ASCII
      phytolamp_state = true;
      sendFeedBack();
      break;  
    }



    else if (inByte == 70){ // "F" == 070 в ASCII
      if (fan_state){
          fan_state = false;
        }
      else{
          fan_state = true;
        }
      sendFeedBack();
      break;
    }
    else if (inByte == 101){ // "e" == 101 в ASCII
      fan_state = false;
      sendFeedBack();
      break;  
    }
    else if (inByte == 102){ // "f" == 102 в ASCII
      fan_state = true;
      sendFeedBack();
      break;  
    }
    

    else if (inByte == 72){ // "H" == 072 в ASCII
      if (air_humiditer_state){
          air_humiditer_state = false;
        }
      else{
          air_humiditer_state = true;
        }
      sendFeedBack();
      break;
    }
    else if (inByte == 103){ // "g" == 103 в ASCII
      air_humiditer_state = false;
      sendFeedBack();
      break;  
    }
    else if (inByte == 104){ // "h" == 104 в ASCII
      air_humiditer_state = true;
      sendFeedBack();
      break;  
    }


    else if (inByte == 67){ // "C" == 067 в ASCII
      if (outlet_fan_state){
          outlet_fan_state = false;
        }
      else{
          outlet_fan_state = true;
        }
      sendFeedBack();
      break;
    }
    else if (inByte == 105){ // "i" == 105 в ASCII
      outlet_fan_state = false;
      sendFeedBack();
      break;  
    }
    else if (inByte == 106){ // "j" == 106 в ASCII
      outlet_fan_state = true;
      sendFeedBack();
      break;  
    }


    else if (inByte == 88){ // "X" == 088 в ASCII
      if (water_heater_state){
          water_heater_state = false;
        }
      else{
          water_heater_state = true;
        }
      sendFeedBack();
      break;
    }
    else if (inByte == 107){ // "k" == 107 в ASCII
      water_heater_state = false;
      sendFeedBack();
      break;  
    }
    else if (inByte == 108){ // "l" == 108 в ASCII
      water_heater_state = true;
      sendFeedBack();
      break;  
    }


    else if (inByte == 89){ // "Y" == 089 в ASCII
      if (water_tank_filler_state){
          water_tank_filler_state = false;
        }
      else{
          water_tank_filler_state = true;
        }
      sendFeedBack();
      break;
    }
    else if (inByte == 109){ // "m" == 109 в ASCII
      water_tank_filler_state = false;
      sendFeedBack();
      break;  
    }
    else if (inByte == 110){ // "n" == 110 в ASCII
      water_tank_filler_state = true;
      sendFeedBack();
      break;  
    }

    

    else if (inByte == 87){ // "W" == 087 в ASCII
      
      // ************* !!! ВНИМАНИЕ !!! НЕОБХОДИМО ОТКРЫТЬ КЛАПАНА !!! *******************
      
      if (pump_state){
          pump_state = false;
        }
      else{
          pump_state = true;
        }
      sendFeedBack();
      break;
    }
    else if (inByte == 111){ // "o" == 111 в ASCII
      pump_state = false;
      sendFeedBack();
      break;  
    }
    else if (inByte == 112){ // "9" == 112 в ASCII
      pump_state = true;
      sendFeedBack();
      break;  
    }

  
 
 }  
  // !!! Исполнение управляющих воздействии от ESP32 !!!
  if(pump_state){digitalWrite(relay1, LOW);}else{digitalWrite(relay1, HIGH);}
  if(air_heater_state){digitalWrite(relay2, LOW);}else{digitalWrite(relay2, HIGH);}
  if(water_heater_state){digitalWrite(relay3, LOW);}else{digitalWrite(relay3, HIGH);}
  if(fan_state){digitalWrite(relay4, LOW);}else{digitalWrite(relay4, HIGH);}
  if(air_humiditer_state){digitalWrite(relay5, LOW);}else{digitalWrite(relay5, HIGH);}
  if(water_tank_filler_state){digitalWrite(relay6, LOW);}else{digitalWrite(relay6, HIGH);}
  if(phytolamp_state){digitalWrite(relay7, LOW);}else{digitalWrite(relay7, HIGH);}
  if(outlet_fan_state){digitalWrite(relay8, LOW);}else{digitalWrite(relay8, HIGH);}
   



  // LED LIGHT CONTROL
  for (int led=0; led<NUM_LEDS; led++){
    leds[led] = CRGB(LED_BRIGHTNESS, 0, LED_BRIGHTNESS);
  }
  FastLED.show();
  
  
  



}   
  
