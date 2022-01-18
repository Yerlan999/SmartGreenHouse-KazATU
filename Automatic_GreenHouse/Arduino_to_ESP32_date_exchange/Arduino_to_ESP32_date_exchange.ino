// КОД ДЛЯ ARDUINO MEGA

#define RXD2 0
#define TXD2 1

String from_ESP32;
String to_ESP32;
String a_ActuatorValue;  
int start_s = 0;
int end_s = 0;

// Тестовые данные датчиков.
float Sensors[5] = {1, -5, 10.99, -230.77, 17};

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1);
}

void loop() {

// *************** БЛОК ОТПРАВКИ ЗНАЧЕНИИИ ДАТЧИКОВ НА ESP32 ***************

  // Конвертация значении датчиков в СТРОКУ!!! для последующей отпавки.
  for(int i=0; i<sizeof(Sensors)/sizeof(float); i++){
    to_ESP32 += String(Sensors[i])+String(",");  // Разделение значении через ","
  };
 
  Serial.println(to_ESP32);   // Отправка данных на ESP32 через "Serial Port"
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
