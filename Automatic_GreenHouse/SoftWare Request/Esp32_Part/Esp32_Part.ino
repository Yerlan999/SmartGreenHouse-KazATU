#define RXD2 16
#define TXD2 17


int baud = 9600;
int waiteTime = 3000;
int pointer = 0;

const unsigned int maxSize = 5;

// Тестовые данные для актуаторов.
int Actuators[5];
int Sensors[10];         // Для хранения значении датчиков с РЕАЛЬНЫМ типом данных

byte from_Arduino;      // Для хранения СТРОКИ со значениями датчиков от Arduino

// Для хранения значении с датчиков
int temperature;
int humidity;
int light;

unsigned long lastTime = 0;  
unsigned long timerDelay = 5000;

// ФУНКЦИЯ ДЛЯ ПОЛУЧЕНИЯ ЗНАЧЕНИИ АКТУАТОРОВ
void collectActuatrosSetValues(){
  Actuators[0] = 1;
  Actuators[1] = 1;
  Actuators[2] = 1;
  Actuators[3] = 1;
  Actuators[4] = 1;
}

// ФУНКЦИЯ ДЛЯ ОТПРАВКИ ЗНАЧЕНИИ АКТУАТОРОВ НА ARDUINO
void sendToArduino(){

  collectActuatrosSetValues();
   
  Serial1.write((uint8_t*)Actuators, sizeof(Actuators));       // Отправка данных на Arduino через "Serial Port"

  while (Serial1.available() > 0){
    from_Arduino = Serial1.read();
    if (from_Arduino == 01000101){     // Binary for "E"
      Serial.print("Actuators delivered!");  
    }
  }
}


// ФУНКЦИЯ ДЛЯ ПРИЕМА ЗНАЧЕНИИ ДАТЧИКОВ ОТ ARDUINO
void recieveFromArduino(){
  
  while (Serial1.available() > 0){
    if (from_Arduino != 0 && pointer < maxSize){
      from_Arduino = Serial1.read();
      Sensors[pointer] = from_Arduino;
      pointer += 1;
    }
  }    
  pointer = 0;
  
  humidity = Sensors[0];
  temperature = Sensors[1];
  light = Sensors[2]; 
}

void setup() {
  Serial.begin(baud);                           // Displaying on monitor                          
  Serial1.begin(baud, SERIAL_8N1, RXD2, TXD2);  // Communication with ARDUINO
}


void loop() {
  
  if ((millis() - lastTime) > timerDelay) {
  
    Serial1.write('A');
  
    while (Serial1.available() > 0){
      int inByte = Serial1.read();
      Sensors[pointer] = inByte;
      Serial.println(inByte);
      pointer += 1;
    }
  
    temperature = Sensors[0];
    humidity = Sensors[2];
    light = Sensors[4];
    pointer = 0;

    Serial.println("******************");
    Serial.printf("%i\n", temperature);
    Serial.println(temperature, BIN);
    Serial.println("******************");
    
    lastTime = millis();
  }
}
