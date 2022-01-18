// КОД ДЛЯ ESP32

#define RXD2 16
#define TXD2 17

String from_Arduino;
String to_Arduino;
String a_SensorValue;  
int start_s = 0;
int end_s = 0;

// Тестовые данные для актуаторов.
float Actuators[5] = {1, -1, 1.99, -1.77, 1};

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() {

// *************** БЛОК ОТПРАВКИ ЗНАЧЕНИИИ АКТУАТОРОВ НА ARDUINO ***************

  // Конвертация значении актуаторов в СТРОКУ!!! для последующей отпавки.
//  for(int i=0; i<sizeof(Actuators)/sizeof(float); i++){
//    to_Arduino += String(Actuators[i])+String(",");  // Разделение значении через ","
//  };
//
//  Serial1.println(to_Arduino);       // Отправка данных на Arduino через "Serial Port"
//  to_Arduino = "";
//  delay(600);
  
// *************************************************************************


// *************** БЛОК ПРИЕМА ЗНАЧЕНИИИ ДАТЧИКОВ ОТ ARDUINO ***************

    // Прием СТРОКИ от Arduino(Значения датчиков).
    from_Arduino = Serial1.readString();
    Serial.println(from_Arduino);       // Вывод всей СТРОКИ.
    
    // Парсинг(Разделение) значении через ",".
//    for(int i=0; i<from_Arduino.length(); i++){
//       if(String(from_Arduino.charAt(i))==String(",")){
//         end_s = i;
//         a_SensorValue = from_Arduino.substring(start_s, end_s);
//         start_s = end_s+1;
//         Serial.println(a_SensorValue);  // !!! Вывод значении каждого датчика.
//       };   
//    };

// *************************************************************************


}
