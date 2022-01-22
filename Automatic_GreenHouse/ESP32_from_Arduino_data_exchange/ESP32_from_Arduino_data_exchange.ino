// КОД ДЛЯ ESP32

#define RXD2 16
#define TXD2 17

String from_Arduino;      // Для хранения СТРОКИ со значениями датчиков от Arduino
String to_Arduino;        // Для хранения данных для Отпавки на Arduino в виде СТРОКИ
String a_SensorValue;     // Для хранения значения отдельного датчика перед конвертацией
int start_s = 0;          // Для парсинга (разбора) СТРОКИ 
int end_s = 0;            // Для парсинга (разбора) СТРОКИ
int comma_Counter = 0;    // Для подчета количества запятых в получаемой от Arduino строке
int found = 5;            // Отражает количество значении переменных получаемых от Arduino

// Тестовые данные для актуаторов.
float Actuators[5] = {1, -1, 1.99, -1.77, 1};
float Sensors[5];         // Для хранения значении датчиков с РЕАЛЬНЫМ типом данных

void setup() {
  Serial.begin(9600);                           // Для вывода на монитор                          
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);  // От и К Arduino
}


void sendToArduino(){

  collectActuatrosSetValues();
  
  // Конвертация значении актуаторов в СТРОКУ!!! для последующей отпавки.
  to_Arduino += "<";
  for(int i=0; i<sizeof(Actuators)/sizeof(float); i++){
    to_Arduino += String(Actuators[i])+String(",");  // Разделение значении через ","
  };
  to_Arduino += ">";

  Serial1.print(to_Arduino);       // Отправка данных на Arduino через "Serial Port"

  Serial.print("Sent: ");
  Serial.println(to_Arduino);
  to_Arduino = "";
  delay(600);
  
}


void recieveFromArduino(){

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
        
  
      // Парсинг(Разделение) значении через ",".
      if(comma_Counter == found){      
        
        Serial.print("Recived: ");
        Serial.println(from_Arduino);      // Вывод всей полученной СТРОКИ.
        
        for(int i=0; i<from_Arduino.length(); i++){
           if(String(from_Arduino.charAt(i))==String(",")){
             end_s = i;
             a_SensorValue = from_Arduino.substring(start_s, end_s);   // Получение значения как строки
             start_s = end_s + 1;
             Sensors[comma_Counter - found] = a_SensorValue.toFloat();   // Конвертация и Составление списка
             found -= 1;
           };                         
        }; start_s = 0; end_s = 0;      
      }; comma_Counter = 0; found = 5;
    }
    // !!! ГОТОВАЯ ПЕРЕМЕННАЯ-СПИСОК "Sensors" СО ЗНАЧЕНИЯМИ ДАТЧИКОВ ПОЛУЧЕННЫЙ ОТ ARDUINO !!!
  
}

void loop() {

// *************** БЛОК ОТПРАВКИ ЗНАЧЕНИИИ АКТУАТОРОВ НА ARDUINO ***************
   sendToArduino();
// *************************************************************************


// *************** БЛОК ПРИЕМА ЗНАЧЕНИИИ ДАТЧИКОВ ОТ ARDUINO ***************
   recieveFromArduino();
// *************************************************************************


}
