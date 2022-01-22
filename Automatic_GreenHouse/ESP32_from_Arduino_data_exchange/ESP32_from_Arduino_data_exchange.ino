// КОД ДЛЯ ESP32

#define TXD2 17
#define RXD2 16

String from_Arduino;      // Для хранения СТРОКИ со значениями датчиков от Arduino
String to_Arduino;        // Для хранения данных для Отпавки на Arduino в виде СТРОКИ
String a_SensorValue;     // Для хранения значения отдельного датчика перед конвертацией
int start_s = 0;          // Для парсинга (разбора) СТРОКИ 
int end_s = 0;            // Для парсинга (разбора) СТРОКИ
int comma_Counter = 0;    // Для подчета количества запятых в получаемой от Arduino строке
int found = 5;            // Отражает количество значении переменных получаемых от Arduino

// Тестовые данные для актуаторов.
float Actuators[5];
float Sensors[5];         // Для хранения значении датчиков с РЕАЛЬНЫМ типом данных

// ФУНКЦИЯ ДЛЯ ПОЛУЧЕНИЯ ЗНАЧЕНИИ АКТУАТОРОВ
void collectActuatrosSetValues(){
  Actuators[0] = 1;
  Actuators[1] = -1;
  Actuators[2] = 1.99;
  Actuators[3] = -1.77;
  Actuators[4] = 1;
}

// ФУНКЦИЯ ДЛЯ ОТПРАВКИ ЗНАЧЕНИИ АКТУАТОРОВ НА ARDUINO
void sendToArduino(){
  
  // Конвертация значении актуаторов в СТРОКУ!!! для последующей отпавки.
  to_Arduino += "<";
  for(int i=0; i<sizeof(Actuators)/sizeof(float); i++){
    to_Arduino += String(Actuators[i])+String(",");  // Разделение значении через ","
  };
  to_Arduino += ">";

  Serial1.print(to_Arduino);       // Отправка данных на Arduino через "Serial Port"
  Serial.println(to_Arduino);      // Вывод на экран
  
  to_Arduino = "";
  delay(500);  
}

// ФУНКЦИЯ ДЛЯ ПРИЕМА ЗНАЧЕНИИ ДАТЧИКОВ ОТ ARDUINO
void getFromArduino(){

    // Прием СТРОКИ от Arduino(Значения датчиков).
    from_Arduino = Serial1.readString();
    
    // Проверка правильности полученных данных (Определение начала и конца)
    if (from_Arduino.startsWith("<") && from_Arduino.endsWith(">")){
      from_Arduino = from_Arduino.substring(1, from_Arduino.indexOf(">"));
      
      // Проверка правильности полученных данных (Подсчет запятых в полученной СТРОКЕ)
      for(int i=0; i<from_Arduino.length(); i++){
        if(String(from_Arduino.charAt(i))==String(",")){
          comma_Counter += 1;    
        };
      };
  
//      Serial.println(from_Arduino);       // Вывод всей СТРОКИ.
  
      // Парсинг(Разделение) значении через ",".
      if(comma_Counter == found){      
        for(int i=0; i<from_Arduino.length(); i++){
           if(String(from_Arduino.charAt(i))==String(",")){
             end_s = i;
             a_SensorValue = from_Arduino.substring(start_s, end_s);   // Получение значения как строки
             start_s = end_s + 1;
             Sensors[comma_Counter - found] = a_SensorValue.toFloat();   // Конвертация и Составление списка
             Serial.println(Sensors[comma_Counter - found]);         // !!! Вывод значении каждого датчика.
             found -= 1;
           };                         
        }; start_s = 0; end_s = 0;      
      }; comma_Counter = 0; found = 5;
  
      // !!! ГОТОВАЯ ПЕРЕМЕННАЯ-СПИСОК "Sensors" СО ЗНАЧЕНИЯМИ ДАТЧИКОВ ПОЛУЧЕННЫЙ ОТ ARDUINO !!!
    }
}

void setup() {
  Serial.begin(9600);                           // Для вывода на монитор                          
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);    // Прием/Отправка на Arduino
}

void loop() {

// *************** БЛОК ОТПРАВКИ ЗНАЧЕНИИИ АКТУАТОРОВ НА ARDUINO ***************
//   collectActuatrosSetValues();
//   sendToArduino ();
// *************************************************************************


// *************** БЛОК ПРИЕМА ЗНАЧЕНИИИ ДАТЧИКОВ ОТ ARDUINO ***************
   getFromArduino();
// *************************************************************************


}
