// КОД ДЛЯ ARDUINO MEGA

#include "DHT.h"

#define RXD2 0
#define TXD2 1

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
//float Sensors[5] = {1, -5, 10.99, -230.77, 17};
float Sensors[5];
float Actuators[5];          // Для хранения значении актуаторов с РЕАЛЬНЫМ типом данных

float temperature;
float humidity;
float pressure;
float moisture;
float water_level;
    
DHT dht(DHTPIN, DHTTYPE);

void getSensorReadings(){
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    pressure = 100.00;
    moisture = -50.41;
    water_level = 5.12;
    
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

  Serial.begin(9600);        // Для вывода на монитор и прием/отправка на ESP32
  Serial1.begin(9600, SERIAL_8N1);
  
  dht.begin();
}

void loop() {

// *************** БЛОК ОТПРАВКИ ЗНАЧЕНИИИ ДАТЧИКОВ НА ESP32 ***************
  
  getSensorReadings();      // Получение значении датчиков
  
  // Конвертация значении датчиков в СТРОКУ!!! для последующей отпавки.
  for(int i=0; i<sizeof(Sensors)/sizeof(float); i++){
    to_ESP32 += String(Sensors[i])+String(",");  // Разделение значении через ","
  };
  
  Serial.print(to_ESP32);   // Отправка данных на ESP32 через "Serial Port"
  to_ESP32 = "";              // Стирание данных.
  delay(1500);
  
// *************************************************************************


// *************** БЛОК ПРИЕМА ЗНАЧЕНИИИ АКТУАТОРОВ ОТ ESP32 ***************

  // Прием СТРОКИ от ESP32(Значения актуаторов).
//    from_ESP32 = Serial.readString();
//    Serial.println(from_ESP32);       // Вывод всей СТРОКИ.
  
    // Парсинг(Разделение) значении через ",".
//    for(int i=0; i<from_ESP32.length(); i++){
//       if(String(from_ESP32.charAt(i))==String(",")){
//         end_s = i;
//         a_ActuatorValue = from_ESP32.substring(start_s, end_s);
//         start_s = end_s+1;
//         Serial.println(a_ActuatorValue);  // !!! Вывод значении каждого актуатора.
//       };   
//    };

// *************************************************************************
  
}
