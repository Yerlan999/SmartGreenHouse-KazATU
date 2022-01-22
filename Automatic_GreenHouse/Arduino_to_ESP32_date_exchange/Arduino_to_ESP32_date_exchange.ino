// КОД ДЛЯ ARDUINO MEGA

#include "DHT.h"

#define DHTPIN 5     
#define DHTTYPE DHT11
#define redLED 2
#define greenLED 3
#define yellowLED 4

String from_ESP32;          // Для хранения СТРОКИ со значениями актуаторов от ESP32
String to_ESP32;            // Для хранения данных для Отпавки на ESP32 в виде СТРОКИ
String a_ActuatorValue;     // Для хранения значения отдельного актуатора перед конвертацией
int start_s = 0;            // Для парсинга (разбора) СТРОКИ 
int end_s = 0;              // Для парсинга (разбора) СТРОКИ 
int comma_Counter = 0;      // Для подчета количества запятых в получаемой от ESP32 строке
int found = 5;              // Отражает количество значении переменных получаемых от ESP32

// Тестовые данные датчиков.
float Sensors[5];
float Actuators[5];          // Для хранения значении актуаторов с РЕАЛЬНЫМ типом данных

// Эмулятор состояния актуаторов
int ten = 0;
int mix_fans = 0;
int light = 0;

float temperature;
float humidity;
float pressure;
float moisture;
float water_level;
    
DHT dht(DHTPIN, DHTTYPE);

// ФУНКЦИЯ ДЛЯ ПОЛУЧЕНИЯ ПОКАЗАНИИ ДАТЧИКОВ
void getSensorReadings(){
  
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    pressure = 100.00;
    moisture = -50.41;
    water_level = 5.12;
    
    Sensors[0] = temperature;   
    Sensors[1] = humidity;
    Sensors[2] = pressure;
    Sensors[3] = moisture;
    Sensors[4] = water_level;
}

// ФУНКЦИЯ ДЛЯ ОТПРАВКИ ЗНАЧЕНИИ ДАТЧИКОВ НА ESP32
void sendToESP32(){

  // Конвертация значении датчиков в СТРОКУ!!! для последующей отпавки.
  to_ESP32 += "<";
  for(int i=0; i<sizeof(Sensors)/sizeof(float); i++){
    to_ESP32 += String(Sensors[i])+String(",");  // Разделение значении через ","
  };
  to_ESP32 += ">";
  
  Serial1.print(to_ESP32);   // Отправка данных на ESP32 через "Serial Port"
  Serial.println(to_ESP32);  // Вывод на экран
  
  to_ESP32 = "";             // Стирание данных.
  delay(1000);
}

// ФУНКЦИЯ ДЛЯ ПРИЕМА ЗНАЧЕНИИ АКТУАТОРОВ ОТ ESP32
void getFromESP32(){

  //Прием СТРОКИ от ESP32(Значения актуаторов).
  from_ESP32 = Serial1.readString();
  
  // Проверка правильности полученных данных (Определение начала и конца)
  if (from_ESP32.startsWith("<") && from_ESP32.endsWith(">")){
    from_ESP32 = from_ESP32.substring(1, from_ESP32.indexOf(">"));
    
    // Проверка правильности полученных данных (Подсчет запятых в полученной СТРОКЕ)
    for(int i=0; i<from_ESP32.length(); i++){
      if(String(from_ESP32.charAt(i))==String(",")){
        comma_Counter += 1;    
      };
    };
    
  //  Serial.println(from_ESP32);       // Вывод всей СТРОКИ.
    
    //Парсинг(Разделение) значении через ",".
    if(comma_Counter == found){
      for(int i=0; i<from_ESP32.length(); i++){
         if(String(from_ESP32.charAt(i))==String(",")){
           end_s = i;
           a_ActuatorValue = from_ESP32.substring(start_s, end_s);   // Получение значения как строки
           start_s = end_s+1;
           Actuators[comma_Counter - found] = a_ActuatorValue.toFloat();  // Конвертация и Составление списка
           Serial.println(Actuators[comma_Counter - found]);  // !!! Вывод значении каждого актуатора.
           found -= 1;
         };   
      }; start_s = 0; end_s = 0;
    }; comma_Counter = 0; found = 5;
  
    // !!! ГОТОВАЯ ПЕРЕМЕННАЯ-СПИСОК "Actuators" СО ЗНАЧЕНИЯМИ ДАТЧИКОВ ПОЛУЧЕННЫЙ ОТ ESP32 !!!
  }  
}

void setup() {
  pinMode(redLED,OUTPUT);
  pinMode(greenLED,OUTPUT);
  pinMode(yellowLED,OUTPUT);

  Serial.begin(9600);        // Для вывода на монитор
  Serial1.begin(9600);       // Прием/Отправка на ESP32
  
  dht.begin();
}

void loop() {

// *************** БЛОК ОТПРАВКИ ЗНАЧЕНИИИ ДАТЧИКОВ НА ESP32 ***************
  getSensorReadings();      // Получение значении датчиков
  sendToESP32();
// *************************************************************************


// *************** БЛОК ПРИЕМА ЗНАЧЕНИИИ АКТУАТОРОВ ОТ ESP32 ***************
//  getFromESP32();
// *************************************************************************
  
}
