// Импортирование необходимых библиотек
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <NTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include "GyverEncoder.h"
#include "RTClib.h"
#include "FS.h"
#include "SD.h"
#include <SPI.h>

/* 
  CLK == S1
  DT == S2
  SW == Key 
*/

#define CLK 33 // S1
#define DT 25  // S2
#define SW 32  // Key
#define countof(L1system_titles) (sizeof(L1system_titles) / sizeof(L1system_titles[0]))
#define RXD2 16
#define TXD2 17
#define ONBOARD_LED  2
#define SD_CS 5


// Назначение статического IP адреса
IPAddress local_IP(192, 168, 72, 248);
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 255);
IPAddress primaryDNS(8, 8, 8, 8);  
IPAddress secondaryDNS(8, 8, 4, 4);

WiFiMulti wifiMulti;

// Объявление объекта NTP Client для получения времени 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


Encoder enc1(CLK, DT, SW, true);
LiquidCrystal_I2C lcd(0x27, 20, 4);

RTC_DS3231 rtc;


// Создание AsyncWebServer объекта на порте 80
AsyncWebServer server(80);

// Создание Event Source объекта
AsyncEventSource events("/events");


int baud = 9600;
int wifi_try_counter = 0;
String gui_control_mode = "web-based"; // "web-based" or "manual"


// Параметры сети WI-FI
const char* main_ssid = "Le petit dejeuner 2";
const char* main_password = "DoesGodReallyExist";

const char* other_ssid = "HUAWEI Y6 2019";
const char* other_password = "mama1963";

const char* another_ssid = "HUAWEI P8 lite 2017";
const char* another_password = "11111111";

const char* http_username = "micro";
const char* http_password = "micro";


// Интервал обновления показании датчиков
unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;    // КАЖДЫЕ 30 секунд

// Переменные для хранения и обработки значении времени для Веб-страницы
String formattedDate;
String dayStamp;
String timeStamp;
String DateTimeStamp;
char datestring[20];

int led_brightness = 15; // from 0 to 255;

// Используются для вывода переданных со страницы данных
const char* FAN_PARAM_INPUT = "new-fan-value";

const char* TEMP_PARAM_INPUT1 = "new-temp-value";
const char* TEMP_PARAM_INPUT2 = "new-temp-value-tog";

const char* HUM_PARAM_INPUT1 = "new-hum-value";
const char* HUM_PARAM_INPUT2 = "new-hum-value-tog";

const char* CARBON_PARAM_INPUT1 = "new-carbon-value";
const char* CARBON_PARAM_INPUT2 = "new-carbon-value-tog";

const char* WATER_TEMP_PARAM_INPUT1 = "new-water-temp-value";
const char* WATER_TEMP_PARAM_INPUT2 = "new-water-temp-value-tog";

const char* WATER_LEVEL_PARAM_INPUT1 = "new-water-level-value";
const char* WATER_LEVEL_PARAM_INPUT2 = "new-water-level-value-tog";


const char* LIGHT_PARAM_INPUT1 = "new-light-value-time";
const char* LIGHT_PARAM_INPUT2 = "new-light-value-duration1";
const char* LIGHT_PARAM_INPUT3 = "new-light-value";
const char* LIGHT_PARAM_INPUT4 = "new-light-value-repeat";
bool light_repeat = false;

const char* LIGHT_PARAM_INPUT5 = "new-light-value-duration2";
const char* LIGHT_PARAM_INPUT6 = "new-light-value-pause";


const char* WATER_PARAM_INPUT1 = "new-water-value-time";
const char* WATER_PARAM_INPUT2 = "new-water-value-duration1";
const char* WATER_PARAM_INPUT3 = "new-water-value";
const char* WATER_PARAM_INPUT4 = "new-water-value-repeat";
bool water_repeat = false;

const char* WATER_PARAM_INPUT5 = "new-water-value-duration2";
const char* WATER_PARAM_INPUT6 = "new-water-value-pause";


const char* TIME_PARAM_INPUT = "new-update-value";
const char* LED_BRIGHTNESS_INPUT = "new-brightness-value";

        
    bool pump_state = false;
    bool air_heater_state = false;
    bool air_humiditer_state = false;
    bool water_heater_state = false;
    bool fan_state = false;
    bool outlet_fan_state = false;
    bool phytolamp_state = false;
    bool water_tank_filler_state = false;



  // Состояния кнопок контроля
  bool temp_button_state = false;
  bool light_button_state = false;
  bool fan_button_state = false;
  bool hum_button_state = false;
  bool water_button_state = false;
  bool carbon_button_state = false;
  bool water_temp_button_state = false;
  bool water_level_button_state = false;
  
  // Состояние карточек отображения показании
  bool is_temp_set = false;
  bool is_hum_set = false;
  bool is_light_set = false;
  bool is_water_set = false;
  bool is_carbon_set = false;
  bool is_water_temp_set = false;
  bool is_water_level_set = false;
  
  
  // Заданные значения для системы
  String to = " >>> ";
  char set_symbol = '*';
  
  String temp_set_value_s;
  String hum_set_value_s;
  String light_set_value_s;
  String water_set_value_s;
  String carbon_set_value_s;
  String water_temp_set_value_s;
  String water_level_set_value_s;
  
  
  float temp_set_value_f;
  float hum_set_value_f;
  float carbon_set_value_f;
  float water_temp_set_value_f;
  float water_level_set_value_f;
  float light_set_value_f;
  float water_set_value_f;
  


// **** Main system variables ****

int temperature = 0;
int humidity = 0;
int carbon = 0;
int water_temperature = 0;
int water_level = 0;
int water = 0;
int light = 0;

  int dummy_temperature;
  int dummy_humidity;
  int dummy_light;
  int dummy_water;
  int dummy_carbon;
  int dummy_water_temperature;
  int dummy_water_level;

bool temperature_set = false;
bool humidity_set = false;
bool carbon_set = false;
bool water_temp_set = false;
bool water_level_set = false;
bool watering_set = false;
bool lighting_set = false;

bool temperature_state_set = false;
bool humidity_state_set = false;
bool carbon_state_set = false;
bool water_temp_state_set = false;
bool water_level_state_set = false;
bool watering_state_set = false;
bool lighting_state_set = false;

int temperature_set_value = 0;
int humidity_set_value = 0;
int carbon_set_value = 0;
int water_temp_set_value = 0;
int water_level_set_value = 0;

int watering_time_hour = 0;
int watering_time_minute = 0;
int watering_duration1 = 0;
int watering_duration2 = 0;
int watering_pause = 0;
bool watering_time_repeat = false;
bool watering_state = false;

bool watering_clock_set = false;
bool watering_inter_set = false;

int lighting_time_hour = 0;
int lighting_time_minute = 0;
int lighting_duration1 = 0;
int lighting_duration2 = 0;
int lighting_pause = 0;
bool lighting_time_repeat = false;
bool lighting_state = false;

bool lighting_clock_set = false;
bool lighting_inter_set = false;

bool temperature_state = false;
bool humidity_state = false;
bool carbon_state = false;
bool water_temp_state = false;
bool water_level_state = false;

bool is_working_temperature = false;
bool is_working_humidity = false;
bool is_working_carbon = false;
bool is_working_water_temp = false;
bool is_working_water_level = false;
bool is_working_light = false;
bool is_working_water = false;


int water_start_time_h;
int water_start_time_m;

bool water_snap_workC = true;
bool water_track_workC = false;

bool water_snap_workI = true;
bool water_track_workI = false;
bool water_snap_pauseI = false;
bool water_track_pauseI = false;


int light_start_time_h;
int light_start_time_m;

bool light_snap_workC = true;
bool light_track_workC = false;

bool light_snap_workI = true;
bool light_track_workI = false;
bool light_snap_pauseI = false;
bool light_track_pauseI = false;


// **** Main Structures for storing Data **** 


struct wifi_template{ 
  uint8_t num;
  const char* ssid;
  const char* password;
};
wifi_template wifi_dict[] {
    {0, main_ssid, main_password},
    {1, other_ssid, other_password},
    {2, another_ssid, another_password},
};



struct case_one_params{ 
  
  int system_val; // 0
  
  int system_set_val; // 1
  
  bool system_state; // 2
  
  bool is_system_set; // 3
  bool is_state_set; // 4
  
  uint8_t system_num; // 5


  int get_value(int which){
    if (which == 0){
      return this->system_val;
    }
    else if(which == 3){
      return int(this->is_system_set);
    }
    else if(which == 1){
      return this->system_set_val;
    }
    else if(which == 2){
      return int(this->system_state);
    }
    else if(which == 4){
      return int(this->is_state_set);
    }  
  }


  void set_value(int which, int new_value){
    if (which == 0){
      this->system_val = new_value;
    }
    else if(which == 3){
      this->is_system_set = bool(new_value);
    }
    else if(which == 1){
      this->system_set_val = new_value;
    }
    else if(which == 2){
      this->system_state = bool(new_value);
    }
    else if(which == 4){
      this->is_state_set = bool(new_value);
    }
  }
};
case_one_params CaseOne[] {
    {temperature, temperature_set_value, temperature_state, temperature_set, temperature_state_set, 0},
    {humidity, humidity_set_value, humidity_state, humidity_set, humidity_state_set, 1},
    {carbon, carbon_set_value, carbon_state, carbon_set, carbon_state_set, 2},
    {water_temperature, water_temp_set_value, water_temp_state, water_temp_set, water_temp_state_set, 3},
    {water_level, water_level_set_value, water_level_state, water_level_set, water_level_state_set, 4},
};


struct case_two_params{ 

  int system_val; // 0
  
  int system_time_h;  // 1
  int system_time_m; // 2
  int system_dur1; // 3
  bool system_rep; // 4

  int system_dur2; // 5
  int system_pause; // 6
  
  bool system_state; // 7
  
  bool is_clock_set; // 8
  bool is_inter_set; // 9
  bool is_state_set; // 10
  
  uint8_t system_num; // 11
  

  int get_value(int which){
    
    if (which == 0){
      return this->system_val;
    }
    
    else if(which == 8){
      return int(this->is_clock_set);
    }
    else if(which == 9){
      return int(this->is_inter_set);
    }
    else if(which == 10){
      return int(this->is_state_set);
    }
    
    else if(which == 1){
      return this->system_time_h;
    }
    else if(which == 2){
      return this->system_time_m;
    }
    else if(which == 3){
      return this->system_dur1;
    }
    else if(which == 4){
      return int(this->system_rep);
    }
    
    else if(which == 7){
      return int(this->system_state);
    }  

    else if(which == 5){
      return int(this->system_dur2);
    }
    else if(which == 6){
      return int(this->system_pause);
    }
  }

  
  void set_value(int which, int new_value){
    if (which == 0){
      this->system_val = new_value;
    }

    else if(which == 8){
      this->is_clock_set = bool(new_value);
    }
    else if(which == 9){
      this->is_inter_set = bool(new_value);
    }
    else if(which == 10){
      this->is_state_set = bool(new_value);
    }

       
    else if(which == 1){
      this->system_time_h = new_value;
    }
    else if(which == 2){
      this->system_time_m = new_value;
    }
    else if(which == 3){
      this->system_dur1 = new_value;
    }
    else if(which == 4){
      this->system_rep = bool(new_value);
    }

    else if(which == 5){
      this->system_dur2 = new_value;
    }
    else if(which == 6){
      this->system_pause = new_value;
    }
    
    else if(which == 7){
      this->system_state = bool(new_value);
    }
  }
};
case_two_params CaseTwo[] {
    {light, lighting_time_hour, lighting_time_minute, lighting_duration1, lighting_time_repeat, lighting_duration2, lighting_pause, lighting_state, lighting_clock_set, lighting_inter_set, lighting_state_set, 5},
    {water, watering_time_hour, watering_time_minute, watering_duration1, watering_time_repeat, watering_duration2, watering_pause, watering_state, watering_clock_set, watering_inter_set, watering_state_set, 6},
};



typedef struct { 
  uint8_t place;
  uint8_t row;
  uint8_t col;
} coordinates;
const coordinates options[] {
    {0, 0, 1},
    {1, 1, 0},
    {2, 1, 11},
    {3, 2, 0},
};


typedef struct { 
  uint8_t place;
  uint8_t row;
  uint8_t col;
} Scoordinates;
const Scoordinates Soptions[] {
    {0, 0, 1},
    {1, 1, 0},
    {2, 2, 0},
    {3, 1, 11},
    {4, 2, 11},
};


typedef struct { 
  uint8_t system_num;
  String system_name;
} L1systems;
const L1systems L1system_titles[] {
    {0, "Temperature"},
    {1, "Humidity"},
    {2, "CO2 Content"},
    {3, "Water Temp."},
    {4, "Water Level"},
    {5, "Lightening"},
    {6, "Watering"},
};

typedef struct { 
  uint8_t system_num;
  String system_name;
} L2systems;
const L2systems L2system_titles[] {
    {0, "Temp Set Mode"},
    {1, "Humid Set Mode"},
    {2, "CO2 Set Mode"},
    {3, "W.Temp. Set Mode"},
    {4, "W.Level Set Mode"},
    {5, "Light Clock"},
    {6, "Water Clock"},
};

typedef struct { 
  uint8_t system_num;
  String system_name;
} L3systems;
const L3systems L3system_titles[] {
    {0, "Temp On-Off"},
    {1, "Humid On-Off"},
    {2, "CO2 On-Off"},
    {3, "W.Temp. On-Off"},
    {4, "W.Level On-Off"},
    {5, "Light Inter."},
    {6, "Water Inter."},
};

typedef struct { 
  uint8_t system_num;
  String system_name;
} L4systems;
const L4systems L4system_titles[] {
    {0, "Light On-Off"},
    {1, "Water On-Off"},
};


typedef struct { 
  uint8_t case_num;
  uint8_t case_level_deepness;
} casedeepness;
const casedeepness case_deepness[] {
    {0, 2},
    {1, 3},
};


int levels_pointer = 0; // CaseOne[0 to 2] or CaseTwo[0 to 3]
int systems_pointer = 0; // from 0 to 6
int places_pointer = 0; // The Devil Knows...

char hover_cursor = '>';   // for mode 0
char select_cursor = '-';  // for mode 1
char switch_cursor = '=';  // for mode 2

int editing_mode = 0;
bool current_case = 0; // 0 for CaseOne, 1 for CaseTwo
int current_case_level_deepness = 2;


bool getFeedBack(){
  
  delay(500);
  if (Serial1.available() > 0){
    bool feedBack = Serial1.read();
    bool grepper1 = Serial1.read();
    bool grepper2 = Serial1.read();
    bool grepper3 = Serial1.read();
    bool grepper4 = Serial1.read();
    bool grepper5 = Serial1.read();
    
//    Serial.print("FeedBack message: ");
//    Serial.println(feedBack);

    if (feedBack){
      return true;
    }
    else{
      return false;
    }  
  }
}


void blinkBuildLED(){
  digitalWrite(ONBOARD_LED,HIGH);
  delay(200);
  digitalWrite(ONBOARD_LED,LOW);  
}


void getSensorsReadings(){

    Serial1.println('S');
//    delay(500);
    
    if (Serial1.available() > 0){
      temperature = Serial1.read();
      humidity = Serial1.read();
      light = Serial1.read();
//      carbon = Serial1.read();
//      water_temperature = Serial1.read();
//      water_level = Serial1.read();
//      water = Serial1.read();
      
      int garbage1 = Serial1.read();int garbage2 = Serial1.read();int garbage3 = Serial1.read();//int garbage4 = Serial1.read();int garbage5 = Serial1.read();int garbage6 = Serial1.read();int garbage7 = Serial1.read();int garbage8 = Serial1.read();
      
      // UPDATING SENSORS VALUES FROM ARDUINO MEGA
      CaseOne[0].set_value(0, temperature);
      CaseOne[1].set_value(0, humidity);
      CaseTwo[0].set_value(0, light);
 
      Serial.println();
      Serial.println("================================ NEW INTERATION =============================================");
 
      Serial.println();
      Serial.println("Recieved Sensors values: ");
      Serial.println();
      Serial.print("Temperature: ");
      Serial.println(temperature);
      Serial.print("Humidity: ");
      Serial.println(humidity);    
      Serial.print("Light: ");
      Serial.println(light);
    }
}


void getActuatorsReadings(){

    Serial1.println('A');
//    delay(500);
    
    if (Serial1.available() > 0){
      pump_state = Serial1.read();
      air_heater_state = Serial1.read();
      air_humiditer_state = Serial1.read();
//      water_heater_state = Serial1.read();
//      fan_state = Serial1.read();
//      outlet_fan_state = Serial1.read();
//      phytolamp_state = Serial1.read();
//      water_tank_filler_state = Serial1.read();

      int garbage1 = Serial1.read();int garbage2 = Serial1.read();int garbage3 = Serial1.read();int garbage4 = Serial1.read();int garbage5 = Serial1.read();int garbage6 = Serial1.read();int garbage7 = Serial1.read();int garbage8 = Serial1.read();
      
      Serial.println();
      Serial.println("Recieved Actuators values: ");
      Serial.println();
      Serial.print("Pump state: ");
      Serial.println(pump_state);
      Serial.print("Air heater state: ");
      Serial.println(air_heater_state);    
      Serial.print("Air humiditer state: ");
      Serial.println(air_humiditer_state);
    }
}


void get_date_time(){
  DateTime now = rtc.now();
  printDateTime(now);
  DateTimeStamp = datestring;
}

void display_time(){
  get_date_time();
  lcd.setCursor(1 , 3);
  lcd.print(DateTimeStamp);
}

int places_pointer_restrictor(){
  
  if (levels_pointer == 0 && systems_pointer == 7){
    return 2;
  };
  
  if (levels_pointer == 0){
    return 0;
  }
  else if (levels_pointer > 0 && systems_pointer < 5){
    return 1;
  }
  else{
    if (levels_pointer == 1){
      return 3;
    }
    else if (levels_pointer == 2){
      return 2;
    }
    else{
      return 1;
    }
  }
}


int value_changer_with_restrictions(String what, int where, int low_end, int hight_end){
  
  int starter_value;
  
  if (what == "time"){
    starter_value = timerDelay/1000;
  }
  else if (what == "brightness"){
    starter_value = led_brightness;
  }
  
  if (where < 0 && starter_value < hight_end){
    starter_value++;
  }
  else if (where > 0 && starter_value > low_end){
    starter_value--;
  };
  
  return starter_value;
}


int value_changer_with_restrictionsONE(int where, int low_end, int hight_end){
  
  int starter_value = CaseOne[systems_pointer].get_value(levels_pointer);
  bool is_system_set = CaseOne[systems_pointer].get_value(3);
  
  if (!is_system_set){
    if (where < 0 && starter_value < hight_end){
      starter_value++;
    }
    else if (where > 0 && starter_value > low_end){
      starter_value--;
    };
  }
  return starter_value;
}


int value_changer_with_restrictionsTWO(int where, int low_end, int hight_end, int which){
  
  int starter_value = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(which);
  bool is_system_set = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(7);

  if (!is_system_set){
    if (where < 0 && starter_value < hight_end){
      starter_value++; 
    }
    else if (where > 0 && starter_value > low_end){
      starter_value--;
    };
  }
  return starter_value;
}


void editing_values(int where){
//  Serial.println("Option of " + String(places_pointer) + " System of " + String(systems_pointer) + " with Editing Mode of " + String(editing_mode) + "Direction where: " + String(where));

  if (systems_pointer == 7 && levels_pointer == 0 && places_pointer == 1){
    timerDelay = value_changer_with_restrictions("time", where, 0, 1800)*1000;
  }
  else if (systems_pointer == 7 && levels_pointer == 0 && places_pointer == 2){
    led_brightness = value_changer_with_restrictions("brightness", where, 0, 255);
    Serial1.println(String(led_brightness));
  }
  
  // LEVEL 1 or LEVEL 2  
  if (levels_pointer == 1 || levels_pointer == 2){
    if (systems_pointer < 5){ // CASE 1
      CaseOne[systems_pointer].set_value(levels_pointer, value_changer_with_restrictionsONE(where, 0, 99));
    }
    else {
      // CASE 2 CLOCK AND INTERVAL MENUES
      
      // CLOCK MENU
      if (levels_pointer == 1 && places_pointer == 1){
        if (editing_mode == 1){ // HOURS
          CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(1, value_changer_with_restrictionsTWO(where, 0, 23, 1));
        }
        else if (editing_mode == 2){ // MINUTES
          CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(2, value_changer_with_restrictionsTWO(where, 0, 59, 2));
        }
      }
      else if (levels_pointer == 1 && places_pointer == 2){ // DURATION 1
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(3, value_changer_with_restrictionsTWO(where, 0, 23, 3));
      }
      else if (levels_pointer == 1 && places_pointer == 3){ // REPEAT
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(4, value_changer_with_restrictionsTWO(where, 0, 1, 4));
      }      
    
      // INTERVAL MENU
      if (levels_pointer == 2 && places_pointer == 1){ // DURATION 2
       CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(5, value_changer_with_restrictionsTWO(where, 0, 23, 5));  
      }
      else if (levels_pointer == 2 && places_pointer == 2){ // PAUSE
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(6, value_changer_with_restrictionsTWO(where, 0, 23, 6));  
      }
    }
    
  }
  // LEVEL 3 EXLUSIVE FOR CASE 2
  else{
    // CASE 2 STATE MENU
    if (systems_pointer != 7){
      CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(7, value_changer_with_restrictionsTWO(where, 0, 1, 7));
    }
  }
}

void update_place(int where){
  if (editing_mode == 0){
    if (where < 0 && places_pointer < places_pointer_restrictor()){
      places_pointer++;
    }
    else if (where > 0 && places_pointer > 0){
      places_pointer--;
    };
  }
  else{  // IN THE EDITING MODE
    // FUNCTION FOR APPLYING EDITING VALUES HERE MUST BE
    editing_values(where);
  }
}

void update_system(int where){
  if (places_pointer == 0){
    if (where < 0 && systems_pointer < countof(L1system_titles)){
      systems_pointer++;
    }
    else if (where > 0 && systems_pointer > 0){
      systems_pointer--;
    };
  
    if (systems_pointer < countof(L1system_titles)-1){
      current_case = 0;
    }
    else{
      current_case = 1;
    }
  }
}

void update_level(){
  if (places_pointer == 0 && systems_pointer != 7){
    if (levels_pointer < case_deepness[current_case].case_level_deepness){
      levels_pointer++;  
    }
    else{
      levels_pointer = 0;
    }  
  }
}

void track_cursor(){
  Serial.println(" ");
  Serial.println("On Level: " + String(levels_pointer)+ "; On System: " +String(systems_pointer) + "; On Place: " + String(places_pointer));
}


void toggle_editing_mode(){
  if (places_pointer != 0){

    if (editing_mode == 1 && systems_pointer > 4 && places_pointer == 1 && levels_pointer == 1){
      editing_mode = 2;
    }
    else if(editing_mode == 2 && systems_pointer > 4 && places_pointer == 1 && levels_pointer == 1){
      editing_mode = 0;
    }
    else if(editing_mode == 0 && systems_pointer > 4 && places_pointer == 1 && levels_pointer == 1){
      editing_mode = 1;
    }

    else if(editing_mode == 0 && systems_pointer > 4 && levels_pointer != 1){
      editing_mode = 1;
    }
    else if(editing_mode == 1 && systems_pointer > 4 && levels_pointer != 1){
      editing_mode = 0;
    }

    else if(editing_mode == 0 && systems_pointer > 4 && levels_pointer == 1){
      editing_mode = 1;
    }
    else if(editing_mode == 1 && systems_pointer > 4 && levels_pointer == 1){
      editing_mode = 0;
    }
    
    else if (editing_mode == 1 && systems_pointer < 5){
      editing_mode = 0;
    }
    else if (editing_mode == 0 && systems_pointer < 5){
      editing_mode = 1;
    }
  }
}


char update_cursor_type(){
  if (editing_mode == 0){
    return hover_cursor;
  }
  else if (editing_mode == 1){
    return select_cursor;
  }
  else{
    return switch_cursor;
  }
}

void update_cursor(){
  if (systems_pointer == 7 && levels_pointer == 0){
    lcd.setCursor(Soptions[places_pointer].col, Soptions[places_pointer].row);
  }
  else{
    lcd.setCursor(options[places_pointer].col, options[places_pointer].row);
  }
  lcd.print(update_cursor_type());  
}

char system_set_icon(){
  if (levels_pointer > 0){
    if (systems_pointer < 5){
      bool is_set = CaseOne[systems_pointer].get_value(levels_pointer+2);
      if (is_set){
        return set_symbol;  
      }    
    }
    else{
      bool is_set = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(levels_pointer+7);
      if (is_set){
        return set_symbol;
      }
    }
    return ' ';
  }
  else{
    return ' ';
  }
}

String current_title(){
  if (systems_pointer == 7 && levels_pointer == 0){
    return "Settings";
  };
  
  switch (levels_pointer) {
  case 0:
    return L1system_titles[systems_pointer].system_name;  
    break;
  case 1:
    return L2system_titles[systems_pointer].system_name; 
    break;
  case 2:
    return L3system_titles[systems_pointer].system_name;
    break;
  case 3:
    return L4system_titles[bool(systems_pointer-(countof(L1system_titles)-2))].system_name;
    break;
}
 
}

void update_title(){
  lcd.setCursor(options[0].col+1, options[0].row);
  lcd.print(current_title());
  lcd.setCursor(19, 0);
  lcd.print(system_set_icon());
}

void update_menu(){

  if (systems_pointer == 7 && levels_pointer == 0){
    lcd.setCursor(Soptions[1].col+1, Soptions[1].row);
    lcd.print("timing:" + String(timerDelay/1000));

    lcd.setCursor(Soptions[2].col+1, Soptions[2].row);
    lcd.print("led bright:" + String(led_brightness));  
    makeSnapShot(7, "g,"+String(timerDelay)+","+String(led_brightness)+",");  
  }
  
  
  // LEVEL 1
  if (levels_pointer == 0 && systems_pointer != 7){
    // CASE 1
    if (systems_pointer < 5){
      lcd.setCursor(options[1].col+1, options[1].row);
      lcd.print("curr value:" + String(CaseOne[systems_pointer].system_val));
    }
    // CASE 2
    else{
      lcd.setCursor(options[1].col+1, options[1].row);
      lcd.print("curr value:" + String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_val));
    }    
  }
  
  // LEVEL 2
  else if (levels_pointer == 1){
    // CASE 1
    if (systems_pointer < 5){
      lcd.setCursor(options[1].col+1, options[1].row);
      lcd.print("set value:" + String(CaseOne[systems_pointer].system_set_val));
    }
    // CASE 2
    else{
      lcd.setCursor(options[1].col+1, options[1].row);
      lcd.print("time:" + String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_time_h) + ":" + String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_time_m));

      lcd.setCursor(options[2].col+1, options[2].row);
      lcd.print("dur:" + String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_dur1));
      
      lcd.setCursor(options[3].col+1, options[3].row);
      lcd.print("repeat:" + String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_rep));
    }
  }

  // LEVEL 3
  else if (levels_pointer == 2){
    // CASE 1
    if (systems_pointer < 5){
      lcd.setCursor(options[1].col+1, options[1].row);
      lcd.print("state:" + String(CaseOne[systems_pointer].system_state));
    }
    // CASE 2
    else{
      lcd.setCursor(options[1].col+1, options[1].row);
      lcd.print("dur:" + String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_dur2));

      lcd.setCursor(options[2].col+1, options[2].row);
      lcd.print("pause:" + String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_pause));
    }
  }

  // LEVEL 4
  else if (levels_pointer == 3){
    // CASE 2
    lcd.setCursor(options[1].col+1, options[1].row);
    lcd.print("state:" + String(CaseTwo[bool(systems_pointer-(countof(L1system_titles)-2))].system_state));
  }
}

void update_display(){
  lcd.clear();
  update_cursor();
  update_title();
  update_menu();
  display_time();  
}




void set_current_system(){
  Serial.println("SET ON System: " + String(systems_pointer) + " Level: " + String(levels_pointer) + " Place: " + String(places_pointer));
  // CASES 1
  if (systems_pointer < 5){
    
    if (levels_pointer == 1){
      bool current_state = CaseOne[systems_pointer].get_value(3);
      bool other_state = CaseOne[systems_pointer].get_value(4);
      
      if (current_state){
        CaseOne[systems_pointer].set_value(3, false);
        makeSnapShot(systems_pointer, "s,"+String(CaseOne[systems_pointer].system_set_val)+","+String(CaseOne[systems_pointer].is_system_set)+",");
      }
      else if (!other_state){
        CaseOne[systems_pointer].set_value(3, true);
        makeSnapShot(systems_pointer, "s,"+String(CaseOne[systems_pointer].system_set_val)+","+String(CaseOne[systems_pointer].is_system_set)+",");
      }
    }
    else if (levels_pointer == 2){
      bool current_state = CaseOne[systems_pointer].get_value(4);
      bool other_state = CaseOne[systems_pointer].get_value(3);
      
      if (current_state){
        CaseOne[systems_pointer].set_value(4, false);
        makeSnapShot(systems_pointer, "o,"+String(CaseOne[systems_pointer].system_state)+","+String(CaseOne[systems_pointer].is_state_set)+",");
      }
      else if (!other_state){
        CaseOne[systems_pointer].set_value(4, true);
        makeSnapShot(systems_pointer, "o,"+String(CaseOne[systems_pointer].system_state)+","+String(CaseOne[systems_pointer].is_state_set)+",");
      }      
  
    }
  
  }
  // CASES 2
  else {
   
    if (levels_pointer == 1){
      bool current_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(8);
      bool other_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(9);
      bool another_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(10);
      
      if (current_state){
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(8, false);
        makeSnapShot(systems_pointer, "c,"+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_time_h)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_time_m)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_dur1)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_rep)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].is_clock_set)+"," );
      }
      else if (!other_state && !another_state){
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(8, true);
        makeSnapShot(systems_pointer, "c,"+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_time_h)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_time_m)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_dur1)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_rep)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].is_clock_set)+"," );
      }  
    }
    else if (levels_pointer == 2){
      bool current_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(9);
      bool other_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(8);
      bool another_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(10);
      
      if (current_state){
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(9, false);
        makeSnapShot(systems_pointer, "i,"+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_dur2)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_pause)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].is_inter_set)+"," );
      }
      else if (!other_state && !another_state){
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(9, true);
        makeSnapShot(systems_pointer, "i,"+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_dur2)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_pause)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].is_inter_set)+"," );
      }      
    }
    else if (levels_pointer == 3){
      bool current_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(10);
      bool other_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(8);
      bool another_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(9);
      
      if (current_state){
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(10, false);
        makeSnapShot(systems_pointer, "k,"+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_state)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].is_state_set)+"," );
      }
      else if (!other_state && !another_state){
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(10, true);
        makeSnapShot(systems_pointer, "k,"+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].system_state)+","+String(CaseTwo[systems_pointer-(countof(L1system_titles)-2)].is_state_set)+"," );
      }      
    }      
  }
}


void initHardTimeModule(){
  if (! rtc.begin()) {
//    Serial.println("Couldn't find RTC");
    while (1);
  }
 
  if (rtc.lostPower()) {
//    Serial.println("RTC lost power, lets set the time!");
    // Задать время для модуля через время системы (ОС) при загрузке скетча
    rtc.adjust(DateTime(__DATE__, __TIME__));
    // Задать время для модуля вручную
    // Январь 21, 2014 и 3 часа ночи:
//     rtc.adjust(DateTime(2022, 5, 2, 11, 45, 0));

  }  
  rtc.adjust(DateTime(__DATE__, __TIME__));  
}



void init_sd_card(){
  Serial.println(" ");
  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed!");Serial.println(" ");
  }
  else {
    Serial.println("Card reader initialized successfully!");Serial.println(" ");
  }
  
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached!");Serial.println(" ");
    return;
  }
  else{
    Serial.println("Card has been found. Ok!");
  }
}



void prepare_main_files(){
  Serial.println(" ");
  // Delete if needed
//  deleteFile(SD, "/sersors_logger.txt");
//  deleteFile(SD, "/actuators_logger.txt");
  
  if(!SD.exists("/sersors_logger.txt")) {
    Serial.println("'/sersors_logger.txt' file doens't exist");
    Serial.println("Creating file...'/sersors_logger.txt'");
    writeFile(SD, "/sersors_logger.txt", "Date,Time,Temperatre,Humidity,Lighting,Watering,WaterTemperature,WaterLevel,CO2 \r\n");Serial.println(" ");
  }
  else {
    Serial.println("'/sersors_logger.txt' file already exists!");Serial.println(" ");
  }

  if(!SD.exists("/actuators_logger.txt")) {
    Serial.println("'/actuators_logger.txt' file doens't exist");
    Serial.println("Creating file...'/actuators_logger.txt'");
    writeFile(SD, "/actuators_logger.txt", "Date,Time,PumpState,AirHeaterState,AirHumiditerState,WaterHeaterState,FanState,OutletFanState,PhytolampState,WaterTankFillerState \r\n");Serial.println(" ");
  }
  else {
    Serial.println("'/actuators_logger.txt' file already exists!");
  }  
}



// Состояния кнопок контроля
struct button_state_creator{

  bool temp_button_state;
  bool hum_button_state;
  bool carbon_button_state;
  bool water_temp_button_state;
  bool water_level_button_state;
  bool light_button_state;
  bool water_button_state;

  bool get_value(int which){

    if (which == 0){
      return this->temp_button_state;
    }
    else if (which == 1){
      return this->hum_button_state;
    }
    else if (which == 2){
      return this->carbon_button_state;
    }
    else if (which == 3){
      return this->water_temp_button_state;
    }
    else if (which == 4){
      return this->water_level_button_state;
    }
    else if (which == 5){
      return this->light_button_state;
    }
    else if (which == 6){
      return this->water_button_state;
    }
  }


  void set_value(int which, bool new_value){

    if (which == 0){
      this->temp_button_state = new_value;
    }
    else if (which == 1){
      this->hum_button_state = new_value;
    }
    else if (which == 2){
      this->carbon_button_state = new_value;
    }
    else if (which == 3){
      this->water_temp_button_state = new_value;
    }
    else if (which == 4){
      this->water_level_button_state = new_value;
    }
    else if (which == 5){
      this->light_button_state = new_value;
    }
    else if (which == 6){
      this->water_button_state = new_value;
    }
  }

};
button_state_creator button_state_class[]{
  {temp_button_state,hum_button_state,carbon_button_state,water_temp_button_state,water_level_button_state,light_button_state,water_button_state},
};








// Состояние карточек отображения показании
struct card_state_creator{

  bool is_temp_set;
  bool is_hum_set;
  bool is_carbon_set;
  bool is_water_temp_set;
  bool is_water_level_set;
  bool is_light_set;
  bool is_water_set;


  bool get_value(int which){

    if (which == 0){
      return this->is_temp_set;
    }
    else if (which == 1){
      return this->is_hum_set;
    }
    else if (which == 2){
      return this->is_carbon_set;
    }
    else if (which == 3){
      return this->is_water_temp_set;
    }
    else if (which == 4){
      return this->is_water_level_set;
    }
    else if (which == 5){
      return this->is_light_set;
    }
    else if (which == 6){
      return this->is_water_set;
    }
  }


  void set_value(int which, bool new_value){

    if (which == 0){
      this->is_temp_set = new_value;
    }
    else if (which == 1){
      this->is_hum_set = new_value;
    }
    else if (which == 2){
      this->is_carbon_set = new_value;
    }
    else if (which == 3){
      this->is_water_temp_set = new_value;
    }
    else if (which == 4){
      this->is_water_level_set = new_value;
    }
    else if (which == 5){
      this->is_light_set = new_value;
    }
    else if (which == 6){
      this->is_water_set = new_value;
    }
  }

};
card_state_creator card_state_class[]{
  {is_temp_set,is_hum_set,is_carbon_set,is_water_temp_set,is_water_level_set,is_light_set,is_water_set},
};





// Текст карточек отображения показании
struct card_text_creator{

  String temp_set_value_s;
  String hum_set_value_s;
  String carbon_set_value_s;
  String water_temp_set_value_s;
  String water_level_set_value_s;
  String light_set_value_s;
  String water_set_value_s;


  String get_value(int which){

    if (which == 0){
      return this->temp_set_value_s;
    }
    else if (which == 1){
      return this->hum_set_value_s;
    }
    else if (which == 2){
      return this->carbon_set_value_s;
    }
    else if (which == 3){
      return this->water_temp_set_value_s;
    }
    else if (which == 4){
      return this->water_level_set_value_s;
    }
    else if (which == 5){
      return this->light_set_value_s;
    }
    else if (which == 6){
      return this->water_set_value_s;
    }
  }


  void set_value(int which, String new_value){

    if (which == 0){
      this->temp_set_value_s = new_value;
    }
    else if (which == 1){
      this->hum_set_value_s = new_value;
    }
    else if (which == 2){
      this->carbon_set_value_s = new_value;
    }
    else if (which == 3){
      this->water_temp_set_value_s = new_value;
    }
    else if (which == 4){
      this->water_level_set_value_s = new_value;
    }
    else if (which == 5){
      this->light_set_value_s = new_value;
    }
    else if (which == 6){
      this->water_set_value_s = new_value;
    }
  }

};
card_text_creator card_text_class[]{
  {temp_set_value_s,hum_set_value_s,carbon_set_value_s,water_temp_set_value_s,water_level_set_value_s,light_set_value_s,water_set_value_s},
};



// ФУНКЦИЯ ДЛЯ СЧИТЫВАНИЯ С ДАТЧИКОВ ПОКАЗАНИИ  (ТЕСТОВАЯ/НАЧАЛЬНАЯ)
void getDummySensorReadings(){
    temperature = dummy_temperature;
    humidity = dummy_humidity;
    light = dummy_light;
    water = dummy_water;
    carbon = dummy_carbon;
    water_temperature = dummy_water_temperature;
    water_level = dummy_water_level;    
}


// ФУНКЦИЯ ДЛЯ КОНВЕРТАЦИИ СТРОКИ В ЦИСЛО ПЛАВАЮЩЕЙ ТОЧКОЙ
float stringToFloat(String s)
{
    char arr[12];
    s.toCharArray(arr, sizeof(arr));
    return atof(arr);
}


// ФУНКЦИЯ ДЛЯ КОНВЕРТАЦИИ СТРОКИ В ЦЕЛОЕ ЦИСЛО
int stringToInt(String s)
{
    char arr[12];
    s.toCharArray(arr, sizeof(arr));
    return atoi(arr);
}


void display_wifi_info(){
  // Вывод IP адреса страницы и данных WiFi точки на LCD дисплей
  lcd.clear();
  
  lcd.setCursor(0, 0);  
  lcd.print(WiFi.localIP());
  
  lcd.setCursor(0, 1);
  lcd.print(WiFi.SSID());
  
  lcd.setCursor(0, 2);
  if (WiFi.SSID() == main_ssid){
    lcd.print(main_password);
  }
  else if (WiFi.SSID() == other_ssid){
    lcd.print(other_password);
  }
  else{
    lcd.print(another_password); 
  }
}




void writeFile(fs::FS &fs, String path, String message) {
  Serial.println(" ");
  Serial.println("Writing to file: " + path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file " + path + " for writing!");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message to file " + path + " is written!");
    Serial.println("(!)Message content written: " + message);
  } else {
    Serial.println("Write to file " + path + " failed!");
  }
  file.close();    
}


void appendFile(fs::FS &fs, String path, String message) {
  Serial.println(" ");
  Serial.println("Appending to file: " + path);
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file " + path + " for appending!");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended to file " + path + "!");
    Serial.println("(!)Message content appended: " + message);
  } else {
    Serial.println("Append failed to file " + path + "!");
  }
  file.close();
}


void readFile(fs::FS &fs, String path, int which_system) {
  Serial.println(" ");
  Serial.println("Reading from file: " + path);
  File file = fs.open(path, FILE_READ);
  if(!file) {
    Serial.println("Failed to open file " + path + " for reading!");
    return;
  }
  
  if (file.available()){
    int set_type = file.read();
    int trash_comma = file.read();
      
    String list = file.readStringUntil('\r');
    Serial.print("(!)Head code: "); Serial.println(set_type);
    Serial.print("(!)Message content read: "); Serial.println(list);
    
    String number;
    int seq = 0;
      
    for (int i=0; i<list.length(); i++){
      
      String character;
      
      if (list[i]==','){        
        
        // Reseting system values
        
        // CASE TWO
        if (set_type == 99){ // clock [c,hour,minute,dur1,rep,is_clock_set,]
          CaseTwo[which_system-5].set_value(seq+1, stringToInt(number));
          if (seq==4){CaseTwo[which_system-5].set_value(8, stringToInt(number));}
        }
        else if (set_type == 105){ // interval [i,dur2,pause,is_inter_set,]
          CaseTwo[which_system-5].set_value(seq+5, stringToInt(number));
          if (seq==2){CaseTwo[which_system-5].set_value(9, stringToInt(number));}
        }
        else if (set_type == 107){ // state [k,state,state_set,]
          CaseTwo[which_system-5].set_value(seq+7, stringToInt(number));
          if (seq==1){CaseTwo[which_system-5].set_value(10, stringToInt(number));}
        }
      
        // CASE ONE
        else if (set_type == 115){ // set [s,system_set_val,is_system_set,]
          if (seq==0){CaseOne[which_system].set_value(1, stringToInt(number));}
          if (seq==1){CaseOne[which_system].set_value(3, stringToInt(number));}
        }
        else if (set_type == 111){ // state [0,state,state_set,]
          if (seq==0){CaseOne[which_system].set_value(2, stringToInt(number));}
          if (seq==1){CaseOne[which_system].set_value(4, stringToInt(number));}
        }        

        // settings
        else if (set_type == 103){
          if (seq==0){timerDelay = stringToInt(number);}
          if (seq==1){led_brightness = stringToInt(number);}         
        }
        
        // relays state
        else if (set_type == 114){
          if (seq==0){is_working_temperature = bool(stringToInt(number));}
          if (seq==1){is_working_humidity = bool(stringToInt(number));}
          if (seq==2){is_working_carbon = bool(stringToInt(number));}
          if (seq==3){is_working_water_temp = bool(stringToInt(number));}
          if (seq==4){is_working_water_level = bool(stringToInt(number));}
          if (seq==5){is_working_light = bool(stringToInt(number));}
          if (seq==6){is_working_water = bool(stringToInt(number));}         
        }

        // clock and interval helper variables values
        else if (set_type == 118){
          if (seq==0) {light_snap_workC = bool(stringToInt(number));}
          if (seq==1) {light_track_workC = bool(stringToInt(number));}
          if (seq==2) {light_snap_workI = bool(stringToInt(number));}
          if (seq==3) {light_track_workI = bool(stringToInt(number));}
          if (seq==4) {light_snap_pauseI = bool(stringToInt(number));}
          if (seq==5) {light_track_pauseI = bool(stringToInt(number));}
          if (seq==6) {water_snap_workC = bool(stringToInt(number));}
          if (seq==7) {water_track_workC = bool(stringToInt(number));}
          if (seq==8) {water_snap_workI = bool(stringToInt(number));}
          if (seq==9) {water_track_workI = bool(stringToInt(number));}
          if (seq==10) {water_snap_pauseI = bool(stringToInt(number));}
          if (seq==11) {water_track_pauseI = bool(stringToInt(number));}       
        }        
        
        number = "";
        seq++;
      }
      else{
        character = list[i];
        number+= character;  
      }
      
    }
    
  }
  file.close();

  // START !!! Applying read system values !!!
  Serial1.println(String(led_brightness)); 
  
  // TEMPERATURE SYSTEM
  is_temp_set = CaseOne[0].is_system_set;
  temp_button_state = CaseOne[0].is_system_set || (CaseOne[0].is_state_set && CaseOne[0].system_state);  
  temp_set_value_s = String(CaseOne[0].system_set_val);
    

  // HUMIDITY SYSTEM  
  is_hum_set = CaseOne[1].is_system_set;
  hum_button_state = CaseOne[1].is_system_set || (CaseOne[1].is_state_set && CaseOne[1].system_state);  
  hum_set_value_s = String(CaseOne[1].system_set_val);
  

  // CARBON SYSTEM 
  is_carbon_set = CaseOne[2].is_system_set;
  carbon_button_state = CaseOne[2].is_system_set || (CaseOne[2].is_state_set && CaseOne[2].system_state);  
  carbon_set_value_s = String(CaseOne[2].system_set_val);
  

  // WATER TEMPERATURE SYSTEM  
  is_water_temp_set = CaseOne[3].is_system_set;
  water_temp_button_state = CaseOne[3].is_system_set || (CaseOne[3].is_state_set && CaseOne[3].system_state);  
  water_temp_set_value_s = String(CaseOne[3].system_set_val);


  // WATER LEVEL SYSTEM  
  is_water_level_set = CaseOne[4].is_system_set;
  water_level_button_state = CaseOne[4].is_system_set || (CaseOne[4].is_state_set && CaseOne[4].system_state);  
  water_level_set_value_s = String(CaseOne[4].system_set_val);


  
  // LIGHTING SYSTEM
  is_light_set = CaseTwo[0].is_clock_set || CaseTwo[0].is_inter_set;
  light_button_state = CaseTwo[0].is_clock_set || CaseTwo[0].is_inter_set || (CaseTwo[0].is_state_set && CaseTwo[0].system_state);
  
  if (CaseTwo[0].is_clock_set){
    if (CaseTwo[0].system_rep){
      light_set_value_s = "Начало с: " + String(CaseTwo[0].system_time_h)+ ":" + String(CaseTwo[0].system_time_m) + " прод: " + String(CaseTwo[0].system_dur1) + " мин" + " (пов.)";  
    }
    else{
      light_set_value_s = "Начало с: " + String(CaseTwo[0].system_time_h)+ ":" + String(CaseTwo[0].system_time_m) + " прод: " + String(CaseTwo[0].system_dur1) + " мин";  
    }
  }
  else if (CaseTwo[0].is_inter_set){
    light_set_value_s = "В течение: " + String(CaseTwo[0].system_dur2) + " с паузой в: " + String(CaseTwo[0].system_pause) + " мин";
  }


    
  // WATERING SYSTEM
  is_water_set = CaseTwo[1].is_clock_set || CaseTwo[1].is_inter_set;
  water_button_state = CaseTwo[1].is_clock_set || CaseTwo[1].is_inter_set || (CaseTwo[1].is_state_set && CaseTwo[1].system_state);
  
  if (CaseTwo[1].is_clock_set){
    if (CaseTwo[1].system_rep){
      water_set_value_s = "Начало с: " + String(CaseTwo[1].system_time_h)+ ":" + String(CaseTwo[1].system_time_m) + " прод: " + String(CaseTwo[1].system_dur1) + " мин" + " (пов.)";  
    }
    else{
      water_set_value_s = "Начало с: " + String(CaseTwo[1].system_time_h)+ ":" + String(CaseTwo[1].system_time_m) + " прод: " + String(CaseTwo[1].system_dur1) + " мин";  
    }
  }
  else if (CaseTwo[1].is_inter_set){
    water_set_value_s = "В течение: " + String(CaseTwo[1].system_dur2) + " с паузой в: " + String(CaseTwo[1].system_pause) + " мин";
  }


  // END !!! Applying read system values !!!

}


void cleanFile(fs::FS &fs, String path) {
  Serial.println(" ");
  Serial.println("Cleaning file: " + path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file " + path + " for cleaning!");
    return;
  }
  if(file.print(' ')) {
    Serial.println("File " + path + " cleaned!");
  } else {
    Serial.println("Cleaning failed for file " + path + "!");
  }
  file.close();
}


void deleteFile(fs::FS &fs, String path){
  if (SD.exists(path)){
  Serial.println(" ");
  if (fs.remove(path)){
    Serial.println("Successfully deleted file " + path + "!");
  }
  else{
    Serial.println("Failed with deleting file" + path + "!");
  }}
}



String filePathCreator(int which_system){
  if (which_system == 7){
    return "/settings.txt";
  }
  else if (which_system == 8){
    return "/relays.txt";
  }
  else if (which_system == 9){
    return "/clock_inter.txt";
  }
  else{
    return "/" + L1system_titles[which_system].system_name + ".txt";
  }
}


void makeSnapShot(int which_system, String values){  
  writeFile(SD, filePathCreator(which_system), values);
}

void readSnapShot(int which_system){
  if (SD.exists(filePathCreator(which_system))){
  readFile(SD, filePathCreator(which_system), which_system);}
}




// ФУНКЦИЯ ДЛЯ ПОДКЛЮЧЕНИЯ WI-FI
void initWiFi() {
  Serial.println(" ");
  // Применение конфигурации для IP адреса
//  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
//    Serial.println("STA Failed to configure");
//  }

    WiFi.onEvent(Wifi_connected,SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(Get_IPAddress, SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(Wifi_disconnected, SYSTEM_EVENT_STA_DISCONNECTED); 
    
    WiFi.mode(WIFI_STA);

    wifiMulti.addAP(main_ssid, main_password);
    wifiMulti.addAP(other_ssid, other_password);
    wifiMulti.addAP(another_ssid, another_password);

    if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println(WiFi.SSID());
      
      // Reading system values if registered  
      readSnapShot(0);
      readSnapShot(1);
      readSnapShot(2);
      readSnapShot(3);
      readSnapShot(4);
      readSnapShot(5);
      readSnapShot(6);
      readSnapShot(7);
      readSnapShot(8);
      readSnapShot(9);
      
      gui_control_mode = "web-based";
      Serial.println(" ");
      Serial.println("Web-Based GUI");
      
      display_wifi_info();
    }
    else{
      
      // Reading system values if registered
      readSnapShot(0);
      readSnapShot(1);
      readSnapShot(2);
      readSnapShot(3);
      readSnapShot(4);
      readSnapShot(5);
      readSnapShot(6);
      readSnapShot(7);
      readSnapShot(8);
      readSnapShot(9);
      
      gui_control_mode = "manual";
      Serial.println(" ");
      Serial.println("Manual GUI");
    }

    timeClient.begin();
    timeClient.setTimeOffset(21600);   // 21600 GMT +6 Для Астаны
}

void wifi_try_counter_incrementer(){
  wifi_try_counter++;
  if (wifi_try_counter > 2){
    wifi_try_counter = 0;
  }
}

void Wifi_connected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println(" ");   
  Serial.println("Successfully connected to Access Point");
  Serial.println("Web-Based GUI");
  gui_control_mode = "web-based";
  // Вывод IP адреса на LCD дисплей
//  display_wifi_info();
}

void Get_IPAddress(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println(" ");
  Serial.println("WIFI is connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  gui_control_mode = "web-based";
//  display_wifi_info();
}

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println(" ");
  Serial.println("Disconnected from WIFI access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  
  // Reading system values if registered
  readSnapShot(0);
  readSnapShot(1);
  readSnapShot(2);
  readSnapShot(3);
  readSnapShot(4);
  readSnapShot(5);
  readSnapShot(6);
  readSnapShot(7);
  readSnapShot(8);  
  readSnapShot(9);
  
  gui_control_mode = "manual";
  
//  Serial.println("Reconnecting...");  
//
//  Serial.println(wifi_try_counter);
//  WiFi.begin(wifi_dict[wifi_try_counter].ssid, wifi_dict[wifi_try_counter].password);
//  wifi_try_counter_incrementer();
//  if (WiFi.status() == WL_CONNECTED){
//    Serial.println("Web-Based GUI Again");
//    gui_control_mode = "web-based";
//    // Вывод IP адреса на LCD дисплей
//    display_wifi_info();  
//  }
  
}


// ФУНКЦИИ ДЛЯ ПОЛУЧЕНИЯ ЗНАЧЕНИИ ДАТЫ И ВРЕМЕНИ
void getWiFiDateTime(){
    
    while(!timeClient.update()) {
      timeClient.forceUpdate();
    }
    formattedDate = timeClient.getFormattedDate();
    
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    // Extract time
    timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-4);    
    DateTimeStamp = dayStamp + " " + timeStamp;
//    Serial.println("Relying on Wifi Time...");
//    Serial.println(DateTimeStamp);    
}


void printDateTime(const DateTime& dt)
{

    snprintf_P(datestring, 
      countof(datestring),
      PSTR("%02u-%02u-%02u %02u:%02u"),
      dt.year(),
      dt.month(),
      dt.day(),
      dt.hour(),
      dt.minute()
      );
//    Serial.print(datestring);
}


void getHardDateTime(){ 
    DateTime now = rtc.now();
    printDateTime(now);
    DateTimeStamp = datestring;    
//    Serial.println();
//    Serial.println("Relying on DS1302 Module...");
//    Serial.println(DateTimeStamp);
}



// ДЛЯ ЗАМЕНЫ %ШАБЛОНОВ% на Веб-странице
String processor(const String& var){

  getDummySensorReadings();
  getWiFiDateTime();
  
  // Значения датчиков для страницы;
  if(var == "TEMPERATURE"){
    return String(temperature);
  }
  else if(var == "DATETIME"){
    return String(DateTimeStamp);
  }
  else if(var == "HUMIDITY"){
    return String(humidity);
  }
  else if(var == "LIGHT"){
    return String(light);
  }
  else if(var == "WATER"){
    return String(water);
  }
  else if(var == "WATER_TEMP"){
    return String(water_temperature);
  }
  else if(var == "WATER_LEVEL"){
    return String(water_level);
  }    
  else if(var == "CARBON"){
    return String(carbon);
  }
    
  // Вывод заданных значении
  else if(var == "TEMP_SET_VALUE"){
    if(is_temp_set){
      return String(to + temp_set_value_s);
    }
    else{
      return String("");
    };
  } 
  else if(var == "HUM_SET_VALUE"){
    if(is_hum_set){
      return String(to + hum_set_value_s);
    }
    else{
      return String("");
    };  
  }
  else if(var == "LIGHT_SET_VALUE"){
    if(is_light_set){
      return String(light_set_value_s);
    }
    else{
      return String("");
    };
  }
  else if(var == "WATER_SET_VALUE"){
    if(is_water_set){
      return String(water_set_value_s);
    }
    else{
      return String("");
    };
  }
  else if(var == "WATER_TEMP_SET_VALUE"){
    if(is_water_temp_set){
      return String(to + water_temp_set_value_s);
    }
    else{
      return String("");
    };
  }
  else if(var == "WATER_LEVEL_SET_VALUE"){
    if(is_water_level_set){
      return String(to + water_level_set_value_s);
    }
    else{
      return String("");
    };
  }
  else if(var == "CARBON_SET_VALUE"){
    if(is_carbon_set){
      return String(to + carbon_set_value_s);
    }
    else{
      return String("");
    };
  }
  
  
  // Состояния текста кнопок задатчиков
  else if (var == "TEMP_SUB_BUT_TEXT"){
    if (temp_button_state){
      return String("Выключить");
    }
    else{
      return String("Включить");
    }
    return String(temp_button_state);
  }
  else if (var == "HUM_SUB_BUT_TEXT"){
    if (hum_button_state){
      return String("Выключить");
    }
    else{
      return String("Включить");
    }
    return String(hum_button_state);
  }
  else if (var == "WATER_SUB_BUT_TEXT"){
    if (water_button_state){
      return String("Выключить");
    }
    else{
      return String("Включить");
    }
    return String(water_button_state);
  }
  else if (var == "WATER_TEMP_SUB_BUT_TEXT"){
    if (water_temp_button_state){
      return String("Выключить");
    }
    else{
      return String("Включить");
    }
    return String(water_temp_button_state);
  }
  else if (var == "WATER_LEVEL_SUB_BUT_TEXT"){
    if (water_level_button_state){
      return String("Выключить");
    }
    else{
      return String("Включить");
    }
    return String(water_level_button_state);
  }
  else if (var == "CARBON_SUB_BUT_TEXT"){
    if (carbon_button_state){
      return String("Выключить");
    }
    else{
      return String("Включить");
    }
    return String(carbon_button_state);
  }
  else if (var == "LIGHT_SUB_BUT_TEXT"){
    if (light_button_state){
      return String("Выключить");
    }
    else{
      return String("Включить");
    }
    return String(light_button_state);
  }
  else if (var == "FAN_SUB_BUT_TEXT"){
    if (fan_button_state){
      return String("Выключить");
    }
    else{
      return String("Включить");
    }
    return String(fan_button_state);
  }
   
  // Состояния кнопок задатчиков
  else if (var == "TEMP_BUTTON_STATE"){
    if (temp_button_state){
      return String("buttonON");
    }
    else{
      return String("buttonOFF");
    }
    return String(temp_button_state);
  }
  else if (var == "HUM_BUTTON_STATE"){
    if (hum_button_state){
      return String("buttonON");
    }
    else{
      return String("buttonOFF");
    }
    return String(hum_button_state);
  }
  else if (var == "FAN_BUTTON_STATE"){
    if (fan_button_state){
      return String("buttonON");
    }
    else{
      return String("buttonOFF");
    }
    return String(fan_button_state);
  }
  else if (var == "LIGHT_BUTTON_STATE"){
    if (light_button_state){
      return String("buttonON");
    }
    else{
      return String("buttonOFF");
    }
    return String(light_button_state);
  }
  else if (var == "WATER_BUTTON_STATE"){
    if (water_button_state){
      return String("buttonON");
    }
    else{
      return String("buttonOFF");
    }
    return String(water_button_state);
  }
  else if (var == "WATER_TEMP_BUTTON_STATE"){
    if (water_temp_button_state){
      return String("buttonON");
    }
    else{
      return String("buttonOFF");
    }
    return String(water_temp_button_state);
  }
  else if (var == "WATER_LEVEL_BUTTON_STATE"){
    if (water_level_button_state){
      return String("buttonON");
    }
    else{
      return String("buttonOFF");
    }
    return String(water_level_button_state);
  }
  else if (var == "CARBON_BUTTON_STATE"){
    if (carbon_button_state){
      return String("buttonON");
    }
    else{
      return String("buttonOFF");
    }
    return String(carbon_button_state);
  }
      
  // Состояния карточек отоброжения значении датчиков
  else if (var == "IS_LIGHT_SET"){
    if (is_light_set){
      return String("card-set");
    }
    else{
      return String("card");
    }
    return String(is_light_set);
  }  
  else if (var == "IS_HUM_SET"){
    if (is_hum_set){
      return String("card-set");
    }
    else{
      return String("card");
    }
    return String(is_hum_set);
  }
  else if (var == "IS_TEMP_SET"){
    if (is_temp_set){
      return String("card-set");
    }
    else{
      return String("card");
    }
    return String(is_temp_set);
  }
  else if (var == "IS_WATER_SET"){
    if (is_water_set){
      return String("card-set");
    }
    else{
      return String("card");
    }
    return String(is_water_set);
  }  
  else if (var == "IS_WATER_LEVEL_SET"){
    if (is_water_level_set){
      return String("card-set");
    }
    else{
      return String("card");
    }
    return String(is_water_level_set);
  }
  else if (var == "IS_WATER_TEMP_SET"){
    if (is_water_temp_set){
      return String("card-set");
    }
    else{
      return String("card");
    }
    return String(is_water_temp_set);
  }  
  else if (var == "IS_CARBON_SET"){
    if (is_carbon_set){
      return String("card-set");
    }
    else{
      return String("card");
    }
    return String(is_carbon_set);
  }
    
  else if (var == "UPDATE_SPAN"){
    return String(timerDelay/1000);  
  }
  else if (var == "LED_BRIGHTNESS"){
    return String(led_brightness);  
  };
  
  return String();
}



// !!! ГЛАВНАЯ HTML СТРАНИЦА !!!
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>KazATU PhytoFarm</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />  
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  
  <style>      
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;
            font-family: 'Open Sans', sans-serif;
            background: ivory;
    }
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .card-set { background-color: #f7c3c3; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
    .buttonOFF {
        padding: 10px 20px;
        font-size: 24px;
        text-align: center;
        outline: none;
        color: #fff;
        background-color: #2f4468;
        border: none;
        border-radius: 5px;
        box-shadow: 0 6px #999;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }  
    .buttonOFF:hover {background-color: #1f2e45}
    .buttonOFF:active {
        background-color: #1f2e45;
        box-shadow: 0 4px #666;
        transform: translateY(2px);
      }  

    .buttonON {
        padding: 10px 20px;
        font-size: 24px;
        text-align: center;
        outline: none;
        color: #fff;
        background-color: #dc143c;
        border: none;
        border-radius: 5px;
        box-shadow: 0 6px #999;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }  
    .buttonON:hover {background-color: #a61b36}
    .buttonON:active {
        background-color: #a61b36;
        box-shadow: 0 4px #666;
        transform: translateY(2px);
    }
    
.button-submmit {
  appearance: none;
  background-color: #FAFBFC;
  border: 1px solid rgba(27, 31, 35, 0.15);
  border-radius: 6px;
  box-shadow: rgba(27, 31, 35, 0.04) 0 1px 0, rgba(255, 255, 255, 0.25) 0 1px 0 inset;
  box-sizing: border-box;
  color: #24292E;
  cursor: pointer;
  display: inline-block;
  font-family: -apple-system, system-ui, "Segoe UI", Helvetica, Arial, sans-serif, "Apple Color Emoji", "Segoe UI Emoji";
  font-size: 14px;
  font-weight: 500;
  line-height: 20px;
  list-style: none;
  padding: 6px 16px;
  position: relative;
  transition: background-color 0.2s cubic-bezier(0.3, 0, 0.5, 1);
  user-select: none;
  -webkit-user-select: none;
  touch-action: manipulation;
  vertical-align: middle;
  white-space: nowrap;
  word-wrap: break-word;
}

.button-submmit:hover {
  background-color: #F3F4F6;
  text-decoration: none;
  transition-duration: 0.1s;
}

.button-submmit:disabled {
  background-color: #FAFBFC;
  border-color: rgba(27, 31, 35, 0.15);
  color: #959DA5;
  cursor: default;
}

.button-submmit:active {
  background-color: #EDEFF2;
  box-shadow: rgba(225, 228, 232, 0.2) 0 1px 0 inset;
  transition: none 0s;
}

.button-submmit:focus {
  outline: 1px transparent;
}

.button-submmit:before {
  display: none;
}

.button-submmit:-webkit-details-marker {
  display: none;
}
  

</style>
</head>

<body>
  <div class="topnav">
    <h1>ФИТОКАМЕРА КАЗАТУ</h1>
    <p style="display:inline;"><i class="far fa-clock" style="color:#e1e437;"></i> Bремя: </p><p style="display:inline;"><span class="reading"><span id="datetime">%DATETIME%</span></span></p>
  </div>
 
  <div class="content">
  
    <!-- РАЗДЕЛ ОТОБРАЖЕНИЯ ПОКАЗАНИИ ДАТЧИКОВ --> 
    <div class="cards">
        <div class=%IS_TEMP_SET%>
          <p><i class="fas fa-thermometer-half" style="color:#f55442;"></i> ТЕМПЕРАТУРА ВОЗДУХА</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span><span>%TEMP_SET_VALUE%</span> &deg;C</span></p>
        </div>
        <div class=%IS_HUM_SET%>
          <p><i class="fas fa-tint" style="color:#00add6;"></i> ВЛАЖНОСТЬ ВОЗДУХА</p><p><span class="reading"><span id="hum">%HUMIDITY%</span><span>%HUM_SET_VALUE%</span> &percnt;</span></p>
        </div>
        <div class=%IS_LIGHT_SET%>
          <p>%LIGHT_SET_VALUE%</p>
          <p><i class="far fa-lightbulb" style="color:#e1e437;"></i> УРОВЕНЬ ОСВЕЩЕНИЯ</p><p><span class="reading"><span id="light">%LIGHT%</span> lux</span></p>
        </div>
        <div class=%IS_WATER_SET%>
          <p>%WATER_SET_VALUE%</p>
          <p><i class="fa fa-shower" style="color:#00add6;"></i> ПОЛИВ</p><p><span class="reading"><span id="water">%WATER%</span> л.</span></p>
        </div>
        <div class=%IS_CARBON_SET%>
          <p><i class="fa fa-leaf" style="color:#059e8a;"></i> СОДЕРЖАНИЕ CO2</p><p><span class="reading"><span id="carbon">%CARBON%</span><span>%CARBON_SET_VALUE%</span> &percnt;</span></p>
        </div>   
        <div class=%IS_WATER_TEMP_SET%>
          <p><i class="fas fa-water" style="color:#f55442;"></i> ТЕМПЕРАТУРА ВОДЫ В БАКЕ</p><p><span class="reading"><span id="water_temp">%WATER_TEMP%</span><span>%WATER_TEMP_SET_VALUE%</span> &deg;C</span></p>
        </div>   
        <div class=%IS_WATER_LEVEL_SET%>
          <p><i class="fas fa-ruler-vertical" style="color:#00add6;"></i> УРОВЕНЬ ВОДЫ В БАКЕ</p><p><span class="reading"><span id="water_level">%WATER_LEVEL%</span><span>%WATER_LEVEL_SET_VALUE%</span> л.</span></p>
        </div>       
    </div>
    
    <br><br>

    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ ОТОПЛЕНИЯ ВОЗДУХА -->
    
    <form id="temp-form" class=%TEMP_BUTTON_STATE% action="/gettemp">
      Температура на : <input type="number" name="new-temp-value">
      <input id="temp-value" class="button-submmit" type="submit" value="Задать">
    </form>
    <form id="temp-form-on" class=%TEMP_BUTTON_STATE% action="/gettemp">
      <input type="hidden" name="new-temp-value-tog" value="toggle-temp">
      <input id="temp-on" class="button-submmit" type="submit" value=%TEMP_SUB_BUT_TEXT%>
    </form>
    
    <br><br>

    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ УВЛАЖНЕНИЯ ВОЗДУХА -->
    
    <form id="hum-form" class=%HUM_BUTTON_STATE% action="/gethum">
      Влажность на : <input type="number" name="new-hum-value">
      <input id="hum-value" class="button-submmit" type="submit" value="Задать">
    </form>
    <form id="hum-form-on" class=%HUM_BUTTON_STATE% action="/gethum">
      <input type="hidden" name="new-hum-value-tog" value="toggle-hum">
      <input id="hum-on" class="button-submmit" type="submit" value=%HUM_SUB_BUT_TEXT%>
    </form>
    
    <br><br>

    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ СЛЕЖЕНИЯ ЗА СОДЕРЖАНИЕМ УГЛЕКИСЛОГО ГАЗА -->
    
    <form id="carbon-form" class=%CARBON_BUTTON_STATE% action="/getcarbon">
      Углекислый газ на : <input type="number" name="new-carbon-value">
      <input id="carbon-value" class="button-submmit" type="submit" value="Задать">
    </form>
    <form id="carbon-form-on" class=%CARBON_BUTTON_STATE% action="/getcarbon">
      <input type="hidden" name="new-carbon-value-tog" value="toggle-carbon">
      <input id="carbon-on" class="button-submmit" type="submit" value=%CARBON_SUB_BUT_TEXT%>
    </form>
    
    <br><br>

    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ СЛЕЖЕНИЯ ЗА ТЕМПЕРАТУРОЙ ВОДЫ В БАКЕ -->
    
    <form id="water-temp-form" class=%WATER_TEMP_BUTTON_STATE% action="/getwatertemp">
      Темп. воды на : <input type="number" name="new-water-temp-value">
      <input id="water-temp-value" class="button-submmit" type="submit" value="Задать">
    </form>
    <form id="water-temp-form-on" class=%WATER_TEMP_BUTTON_STATE% action="/getwatertemp">
      <input type="hidden" name="new-water-temp-value-tog" value="toggle-water-temp">
      <input id="water-temp-on" class="button-submmit" type="submit" value=%WATER_TEMP_SUB_BUT_TEXT%>
    </form>
    
    <br><br>

    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ СЛЕЖЕНИЯ ЗА УРОВНЕМ ВОДЫ В БАКЕ -->
    
    <form id="water-level-form" class=%WATER_LEVEL_BUTTON_STATE% action="/getwaterlevel">
      Уров. воды на : <input type="number" name="new-water-level-value">
      <input id="water-level-value" class="button-submmit" type="submit" value="Задать">
    </form>
    <form id="water-level-form-on" class=%WATER_LEVEL_BUTTON_STATE% action="/getwaterlevel">
      <input type="hidden" name="new-water-level-value-tog" value="toggle-water-level">
      <input id="water-level-on" class="button-submmit" type="submit" value=%WATER_LEVEL_SUB_BUT_TEXT%>
    </form>
    
    <br><br>



    
    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ ОСВЕЩЕНИЯ -->

    <div id="flip-light-control-top">
      <form id="light-form" class=%LIGHT_BUTTON_STATE% action="/getlight">
        Начало освещ.: <input type="time" id="time" name="new-light-value-time">
        продолж: <input type="number" name="new-light-value-duration1">
        повтор: <input type="checkbox" name="new-light-value-repeat">
        <input id="light-value" class="button-submmit" type="submit" value="Задать">
      </form>
    </div>

    <div id="flip-light-control-back">
      <form id="light-form" class=%LIGHT_BUTTON_STATE% action="/getlight">
        Продолж. освещ: <input type="number" name="new-light-value-duration2">
        пауза: <input type="number" name="new-light-value-pause">
        <input id="light-value" class="button-submmit" type="submit" value="Задать">
      </form>   
    </div>

    <form id="light-form-on" class=%LIGHT_BUTTON_STATE% action="/getlighttog">
      <input type="hidden" name="new-light-value" value="toggle-light">
      <input id="light-on" class="button-submmit" type="submit" value=%LIGHT_SUB_BUT_TEXT%>
    </form>

    
    <br><br>


    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ ПОЛИВА -->
    
    <div id="flip-water-control-top">
      <form id="water-form" class=%WATER_BUTTON_STATE% action="/getwater">
        Начало полива: <input type="time" id="time" name="new-water-value-time">
        продолж: <input type="number" name="new-water-value-duration1">
        повтор: <input type="checkbox" name="new-water-value-repeat">
        <input id="water-value" class="button-submmit" type="submit" value="Задать">
      </form>
    </div>

    <div id="flip-water-control-back">
      <form id="water-form" class=%WATER_BUTTON_STATE% action="/getwater">
        Продолж. полива: <input type="number" name="new-water-value-duration2">
        пауза: <input type="number" name="new-water-value-pause">
        <input id="water-value" class="button-submmit" type="submit" value="Задать">
      </form>
    </div>

    <form id="water-form-on" class=%WATER_BUTTON_STATE% action="/getwatertog">
      <input type="hidden" name="new-water-value" value="toggle-water">
      <input id="water-on" class="button-submmit" type="submit" value=%WATER_SUB_BUT_TEXT%>
    </form>


    <!--

    <br><br>

      КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ ВЕНТИЛЯЦИИ 
      
      <form id="fan-form-on" class=%FAN_BUTTON_STATE% action="/getfan">
        Вентилятор:<br>
        <input type="hidden" name="new-fan-value" value="toggle-fan">
        <input id="fan-on" class="button-submmit" type="submit" value=%FAN_SUB_BUT_TEXT%>
      </form>
      
    -->
    
    <br><br>
    
    <button type="button" class="button-submmit"><a href="/settings">Настройки</a></button>

  
  </div>


<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
//  console.log("message", e.data);
 }, false);

 source.addEventListener('datetime', function(e) {
//  console.log("datetime", e.data);
  document.getElementById("datetime").innerHTML = e.data;
 }, false);

 source.addEventListener('temperature', function(e) {
//  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);
 
 source.addEventListener('humidity', function(e) {
//  console.log("humidity", e.data);
  document.getElementById("hum").innerHTML = e.data;
 }, false);
 
 source.addEventListener('light', function(e) {
//  console.log("light", e.data);
  document.getElementById("light").innerHTML = e.data;
 }, false);

source.addEventListener('refresher', function(e) {
//  console.log("myevent", e.data);
  window.location = '/';
  }, false)

 document.addEventListener('DOMContentLoaded', function(e){
  console.log("LOADED!!!");
  console.log(document.URL);
  console.log(document.URL.slice(0, 22));
  
  if (document.URL.length > 24){
    window.location = '/';
  }
  else{
    console.log("shorter than 22");  
  }
 }, false);



document.getElementById("flip-light-control-top").addEventListener("dblclick", flipper.bind(event, "flip-light"));
document.getElementById("flip-light-control-back").addEventListener("dblclick", flipper.bind(event, "flipback-light"));
document.getElementById("flip-water-control-top").addEventListener("dblclick", flipper.bind(event, "flip-water"));
document.getElementById("flip-water-control-back").addEventListener("dblclick", flipper.bind(event, "flipback-water"));

document.getElementById("flip-light-control-back").style.setProperty("display", "none");
document.getElementById("flip-water-control-back").style.setProperty("display", "none");

function flipper(flag){
  if (flag == "flip-light"){
    console.log("CLICK ON LIGHT CONTROL TOOLS A"); 
    document.getElementById("flip-light-control-back").style.setProperty("display", "block");
    document.getElementById("flip-light-control-top").style.setProperty("display", "none");
  }
  else if (flag == "flipback-light"){
    console.log("CLICK ON LIGHT CONTROL TOOLS B"); 
    document.getElementById("flip-light-control-top").style.setProperty("display", "block");
    document.getElementById("flip-light-control-back").style.setProperty("display", "none");    
  }

  else if (flag == "flip-water"){
    console.log("CLICK ON WATER CONTROL TOOLS A"); 
    document.getElementById("flip-water-control-back").style.setProperty("display", "block");    
    document.getElementById("flip-water-control-top").style.setProperty("display", "none");
  }
  else if (flag == "flipback-water"){
    console.log("CLICK ON WATER CONTROL TOOLS B"); 
    document.getElementById("flip-water-control-top").style.setProperty("display", "block");
    document.getElementById("flip-water-control-back").style.setProperty("display", "none");    
  }  
}





}



</script>
</body>
</html>)rawliteral";



// !!! HTML СТРАНИЦА НАСТРОЕК !!!
const char settings_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>KazATU PhytoFarm</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />  
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .card-set { background-color: #f7c3c3; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
    
.button-submmit {
  appearance: none;
  background-color: #FAFBFC;
  border: 1px solid rgba(27, 31, 35, 0.15);
  border-radius: 6px;
  box-shadow: rgba(27, 31, 35, 0.04) 0 1px 0, rgba(255, 255, 255, 0.25) 0 1px 0 inset;
  box-sizing: border-box;
  color: #24292E;
  cursor: pointer;
  display: inline-block;
  font-family: -apple-system, system-ui, "Segoe UI", Helvetica, Arial, sans-serif, "Apple Color Emoji", "Segoe UI Emoji";
  font-size: 14px;
  font-weight: 500;
  line-height: 20px;
  list-style: none;
  padding: 6px 16px;
  position: relative;
  transition: background-color 0.2s cubic-bezier(0.3, 0, 0.5, 1);
  user-select: none;
  -webkit-user-select: none;
  touch-action: manipulation;
  vertical-align: middle;
  white-space: nowrap;
  word-wrap: break-word;
}

.button-submmit:hover {
  background-color: #F3F4F6;
  text-decoration: none;
  transition-duration: 0.1s;
}

.button-submmit:disabled {
  background-color: #FAFBFC;
  border-color: rgba(27, 31, 35, 0.15);
  color: #959DA5;
  cursor: default;
}

.button-submmit:active {
  background-color: #EDEFF2;
  box-shadow: rgba(225, 228, 232, 0.2) 0 1px 0 inset;
  transition: none 0s;
}

.button-submmit:focus {
  outline: 1px transparent;
}

.button-submmit:before {
  display: none;
}

.button-submmit:-webkit-details-marker {
  display: none;
}

</style>
</head>

<body>
  <div class="topnav">
    <h1>ФИТОКАМЕРА КАЗАТУ</h1>
    <p style="display:inline;"><i class="far fa-clock" style="color:#e1e437;"></i> Bремя: </p><p style="display:inline;"><span class="reading"><span id="datetime">%DATETIME%</span></span></p>
  </div>
 
  <div class="content">
  
    <!-- РАЗДЕЛ ОТОБРАЖЕНИЯ ПОКАЗАНИИ ДАТЧИКОВ --> 
    <div class="cards">
        <div class=%IS_TEMP_SET%>
          <p><i class="fas fa-thermometer-half" style="color:#f55442;"></i> ТЕМПЕРАТУРА ВОЗДУХА</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span><span>%TEMP_SET_VALUE%</span> &deg;C</span></p>
        </div>
        <div class=%IS_HUM_SET%>
          <p><i class="fas fa-tint" style="color:#00add6;"></i> ВЛАЖНОСТЬ ВОЗДУХА</p><p><span class="reading"><span id="hum">%HUMIDITY%</span><span>%HUM_SET_VALUE%</span> &percnt;</span></p>
        </div>
        <div class=%IS_LIGHT_SET%>
          <p>%LIGHT_SET_VALUE%</p>
          <p><i class="far fa-lightbulb" style="color:#e1e437;"></i> УРОВЕНЬ ОСВЕЩЕНИЯ</p><p><span class="reading"><span id="light">%LIGHT%</span> lux</span></p>
        </div>
        <div class=%IS_WATER_SET%>
          <p>%WATER_SET_VALUE%</p>
          <p><i class="fa fa-shower" style="color:#00add6;"></i> ПОЛИВ</p><p><span class="reading"><span id="water">%WATER%</span> л.</span></p>
        </div>
        <div class=%IS_CARBON_SET%>
          <p><i class="fa fa-leaf" style="color:#059e8a;"></i> СОДЕРЖАНИЕ CO2</p><p><span class="reading"><span id="carbon">%CARBON%</span><span>%CARBON_SET_VALUE%</span> &percnt;</span></p>
        </div>   
        <div class=%IS_WATER_TEMP_SET%>
          <p><i class="fas fa-water" style="color:#f55442;"></i> ТЕМПЕРАТУРА ВОДЫ В БАКЕ</p><p><span class="reading"><span id="water_temp">%WATER_TEMP%</span><span>%WATER_TEMP_SET_VALUE%</span> &deg;C</span></p>
        </div>   
        <div class=%IS_WATER_LEVEL_SET%>
          <p><i class="fas fa-ruler-vertical" style="color:#00add6;"></i> УРОВЕНЬ ВОДЫ В БАКЕ</p><p><span class="reading"><span id="water_level">%WATER_LEVEL%</span><span>%WATER_LEVEL_SET_VALUE%</span> л.</span></p>
        </div>       
    </div>
    
    <br><br>

    <form id="update-form" class="" action="/getupd">
      Интервал обновления(сек) : <input type="number" name="new-update-value" placeholder=%UPDATE_SPAN%>
      <input id="update-value" class="button-submmit" type="submit" value="Задать">
    </form>
    
    <br><br>

    <form id="update-form" class="" action="/getupd">
      Яркость светодиодной ленты: <input type="number" name="new-brightness-value" placeholder=%LED_BRIGHTNESS%>
      <input id="update-value" class="button-submmit" type="submit" value="Задать">
    </form>
    
    <br><br>

   
    <button type="button" class="button-submmit"><a href="/">Главная</a></button>

  </div>




<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);

 source.addEventListener('datetime', function(e) {
  console.log("datetime", e.data);
  document.getElementById("datetime").innerHTML = e.data;
 }, false);

 source.addEventListener('temperature', function(e) {
  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);
 
 source.addEventListener('humidity', function(e) {
  console.log("humidity", e.data);
  document.getElementById("hum").innerHTML = e.data;
 }, false);
 
 source.addEventListener('pressure', function(e) {
  console.log("pressure", e.data);
  document.getElementById("pres").innerHTML = e.data;
 }, false);

 document.addEventListener('DOMContentLoaded', function(e){
  console.log("LOADED!!!");
  
 }, false);



source.addEventListener('refresher', function(e) {
//  console.log("myevent", e.data);
  window.location = "/settings";
  }, false)

 document.addEventListener('DOMContentLoaded', function(e){
  console.log("LOADED!!!");
  console.log(document.URL);
  console.log(document.URL.slice(0, 31));
  
  if (document.URL.length > 33){
    window.location = "/settings";
  }
  else{
    console.log("shorter than 22");  
  }
 }, false);


}


</script>
</body>
</html>)rawliteral";


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}


void reset_light(){
  is_working_light = false;
  light_snap_workC = true;
  light_track_workC = false;
  light_snap_workI = true;
  light_track_workI = false;
  light_snap_pauseI = false;
  light_track_pauseI = false;  
}

void reset_water(){
  water_snap_workC = true;
  water_track_workC = false;
  
  water_snap_workI = true;
  water_track_workI = false;
  water_snap_pauseI = false;
  water_track_pauseI = false; 
}

bool compareTimes(int start_time_h, int start_time_m, int right_now_hour, int right_now_minute, int duration){
  int time_laps_inMins = (right_now_hour - start_time_h)*60 + (right_now_minute - start_time_m);  
  
  Serial.println(" ");
  Serial.print("Time Laps: ");
  Serial.println(time_laps_inMins);
  
  if (time_laps_inMins >= duration){
    return true;
  }
  else {
    return false;
  }
}


void TrackSystems(){
  String right_now_time = DateTimeStamp.substring(DateTimeStamp.indexOf(" ")+1, -1);
  String right_now_date = DateTimeStamp.substring(0, DateTimeStamp.indexOf(" "));
  
  appendFile(SD, "/sersors_logger.txt", right_now_date+","+right_now_time+","+String(CaseOne[0].system_val)+","+String(CaseOne[1].system_val)+","+String(CaseOne[2].system_val)+","+String(CaseOne[3].system_val)+","+String(CaseOne[4].system_val)+","+String(CaseTwo[0].system_val)+","+String(CaseTwo[1].system_val)+", \r\n");
  
  
  // TEMPERAURE
  if (CaseOne[0].is_system_set){
    if ((CaseOne[0].system_set_val > CaseOne[0].system_val) && !is_working_temperature){
      Serial1.println("a");
      is_working_temperature = true;              
    }
    else if ((CaseOne[0].system_set_val > CaseOne[0].system_val) && is_working_temperature){
      Serial.println(" ");
      Serial.println("CONTINUE CONTROL ON TEMPERATURE");       
    }
    else{
      Serial1.println("b");
      is_working_temperature = false;
    }
  }
  else if ((CaseOne[0].is_state_set && CaseOne[0].system_state) && !is_working_temperature){
    Serial1.println("a");
    is_working_temperature = true; 
  }
  else if (!CaseOne[0].is_state_set && is_working_temperature){
    Serial1.println("b");
    is_working_temperature = false;
  };
  

  // HUMIDITY
  if (CaseOne[1].is_system_set){
    if ((CaseOne[1].system_set_val > CaseOne[1].system_val) && !is_working_humidity){
      Serial1.println("g");
      is_working_humidity = true;              
    }
    else if ((CaseOne[1].system_set_val > CaseOne[1].system_val) && is_working_humidity){
      Serial.println(" ");
      Serial.println("CONTINUE CONTROL ON HUMIDITY");      
    }
    else{
      Serial1.println("h");
      is_working_humidity = false;
    }
  }
  else if ((CaseOne[1].is_state_set && CaseOne[1].system_state) && !is_working_humidity){
    Serial1.println("g");
    is_working_humidity = true; 
  }
  else if (!CaseOne[1].is_state_set && is_working_humidity){
    Serial1.println("h");
    is_working_humidity = false;
  };


  // CARBON
  if (CaseOne[2].is_system_set){
    if ((CaseOne[2].system_set_val < CaseOne[2].system_val) && !is_working_carbon){
      Serial1.println("i");
      is_working_carbon = true;              
    }
    else if ((CaseOne[2].system_set_val < CaseOne[2].system_val) && is_working_carbon){
      Serial.println(" ");
      Serial.println("CONTINUE CONTROL ON CARBON");       
    }
    else{
      Serial1.println("j");
      is_working_carbon = false;
    }
  }
  else if ((CaseOne[2].is_state_set && CaseOne[2].system_state) && !is_working_carbon){
    Serial1.println("i");
    is_working_carbon = true; 
  }
  else if (!CaseOne[2].is_state_set && is_working_carbon){
    Serial1.println("j");
    is_working_carbon = false;
  };


  // WATER TEMPERAURE
  if (CaseOne[3].is_system_set){
    if ((CaseOne[3].system_set_val > CaseOne[3].system_val) && !is_working_water_temp){
      Serial1.println("k");
      is_working_water_temp = true;              
    }
    else if((CaseOne[3].system_set_val > CaseOne[3].system_val) && is_working_water_temp){
      Serial.println(" ");
      Serial.println("CONTINUE CONTROL ON WATER TEMPEATURE");    
    }
    else{
      Serial1.println("l");
      is_working_water_temp = false;
    }
  }
  else if ((CaseOne[3].is_state_set && CaseOne[3].system_state) && !is_working_water_temp){
    Serial1.println("k");
    is_working_water_temp = true; 
  }
  else if (!CaseOne[3].is_state_set && is_working_water_temp){
    Serial1.println("l");
    is_working_water_temp = false;
  };

  
  // WATER LEVEL
  if (CaseOne[4].is_system_set){
    if ((CaseOne[4].system_set_val > CaseOne[4].system_val) && !is_working_water_level){
      Serial1.println("m");
      is_working_water_level = true;              
    }
    else if ((CaseOne[4].system_set_val > CaseOne[4].system_val) && is_working_water_level){
      Serial.println(" ");
      Serial.println("CONTINUE CONTROL ON WATER LEVEL");
    }
    else{
      Serial1.println("n");
      is_working_water_level = false;
    }
  }
  else if ((CaseOne[4].is_state_set && CaseOne[4].system_state) && !is_working_water_level){
    Serial1.println("m");
    is_working_water_level = true; 
  }
  else if (!CaseOne[4].is_state_set && is_working_water_level){
    Serial1.println("n");
    is_working_water_level = false;
  };



  // LIGHTING
  if ((CaseTwo[0].is_state_set && CaseTwo[0].system_state) && !is_working_light){
    Serial.println(" ");
    Serial.println("Turning ON LIGHTS (TOGGLE)");
    Serial1.println("c");
    is_working_light = true;
    reset_light();
  }
  else if (!CaseTwo[0].is_state_set && is_working_light && !CaseTwo[0].is_clock_set && !CaseTwo[0].is_inter_set){
    Serial.println(" ");
    Serial.println("Turning OFF LIGHTS (TOGGLE)");
    Serial1.println("d");
    reset_light();
  }
  
  else if (CaseTwo[0].is_clock_set){
    Serial.println(" ");
    Serial.println("CLOCK LIGHTS IS SET");
        
    String right_now = DateTimeStamp.substring(DateTimeStamp.indexOf(" ")+1, -1);
    int right_now_minute = stringToInt(right_now.substring(right_now.indexOf(":")+1, -1));
    int right_now_hour = stringToInt(right_now.substring(0, right_now.indexOf(":")));
  
    
    if (right_now_hour >= CaseTwo[0].system_time_h && ((right_now_minute - CaseTwo[0].system_time_m) >= 0) && (right_now_minute >= CaseTwo[0].system_time_m)){
      Serial.println(" ");
      Serial.println("LIGHT (CLOCK) IS MET TIME REQUIREMENTS");
      if (light_snap_workC){
        light_start_time_h = right_now_hour;
        light_start_time_m = right_now_minute;
        
        light_snap_workC = false;
        light_track_workC = true;
        
        Serial.println(" ");
        Serial.println("(!!!)Started tracking! (clock)");
        Serial1.println("c");
        is_working_light = true;
      }
      // tracking time
      if (compareTimes(light_start_time_h, light_start_time_m, right_now_hour, right_now_minute, CaseTwo[0].system_dur1) && light_track_workC){
        Serial.println(" ");
        Serial.println("(!!!)Completed tracking by min of duration! (clock)");
        Serial1.println("d");
        is_working_light = false;
        if (CaseTwo[0].system_rep){
          light_snap_workC = true;
          light_track_workC = false;        
        }
        else{
          light_snap_workC = true;
          light_track_workC = false; 
          CaseTwo[0].set_value(8, false);
          
          is_light_set = false;
          light_button_state = false;
          events.send("Refresh the page","refresher",millis());
        }  
      }
    }
  
  makeSnapShot(9, "v,"+String(int(light_snap_workC))+","+String(int(light_track_workC))+","+String(int(light_snap_workI))+","+String(int(light_track_workI))+","+String(int(light_snap_pauseI))+","+String(int(light_track_pauseI))+","+String(int(water_snap_workC))+","+String(int(water_track_workC))+","+String(int(water_snap_workI))+","+String(int(water_track_workI))+","+String(int(water_snap_pauseI))+","+String(int(water_track_pauseI))+",");    
  }
  
  else if (CaseTwo[0].is_inter_set){
    Serial.println(" ");
    Serial.println("INTERVAL LIGHTS IS SET");
        
    String right_now = DateTimeStamp.substring(DateTimeStamp.indexOf(" ")+1, -1);
    int right_now_minute = stringToInt(right_now.substring(right_now.indexOf(":")+1, -1));
    int right_now_hour = stringToInt(right_now.substring(0, right_now.indexOf(":")));
    
    if (light_snap_workI){
      light_start_time_h = right_now_hour;
      light_start_time_m = right_now_minute;
      
      light_snap_workI = false;
      light_track_workI = true;
    
      Serial.println(" ");
      Serial.println("(!!!)Started tracking! (interval)");
      Serial1.println("c");
      is_working_light = true;
    }
    // tracking time
    if (light_track_workI){
      if (compareTimes(light_start_time_h, light_start_time_m, right_now_hour, right_now_minute, CaseTwo[0].system_dur2)){
        
        Serial.println(" ");
        Serial.println("(!!!)Completed tracking by min of duration! (interval)");
        Serial1.println("d");
        is_working_light = false;
        light_track_workI = false;
        light_snap_pauseI = true;          
      }
    }
    else if (light_snap_pauseI){
      light_start_time_h = right_now_hour;
      light_start_time_m = right_now_minute;
      
      Serial.println(" ");
      Serial.println("(!!!)Started pausing! (interval)");
      light_snap_pauseI = false; 
      light_track_pauseI = true;     
    }
    // tracking pause
    else if(light_track_pauseI){
      if (compareTimes(light_start_time_h, light_start_time_m, right_now_hour, right_now_minute, CaseTwo[0].system_pause)){
        
        Serial.println(" ");
        Serial.println("(!!!)Completed tracking by min of pause! (interval)");
        light_snap_workI = true;
        light_track_pauseI = false;  
      }    
    }
    
  makeSnapShot(9, "v,"+String(int(light_snap_workC))+","+String(int(light_track_workC))+","+String(int(light_snap_workI))+","+String(int(light_track_workI))+","+String(int(light_snap_pauseI))+","+String(int(light_track_pauseI))+","+String(int(water_snap_workC))+","+String(int(water_track_workC))+","+String(int(water_snap_workI))+","+String(int(water_track_workI))+","+String(int(water_snap_pauseI))+","+String(int(water_track_pauseI))+",");
  };



  // WATERING
  if ((CaseTwo[1].is_state_set && CaseTwo[1].system_state) && !is_working_water){
    Serial.println(" ");
    Serial.println("Turning ON WATER (TOGGLE)");
    Serial1.println("c");
    is_working_water = true;
    reset_water();
  }
  else if (!CaseTwo[1].is_state_set && is_working_water && !CaseTwo[1].is_clock_set && !CaseTwo[1].is_inter_set){
    Serial.println(" ");
    Serial.println("Turning OFF WATER (TOGGLE)");
    Serial1.println("d");
    reset_water();
  }
  
  else if (CaseTwo[1].is_clock_set){
    Serial.println(" ");
    Serial.println("CLOCK WATER IS SET");
        
    String right_now = DateTimeStamp.substring(DateTimeStamp.indexOf(" ")+1, -1);
    int right_now_minute = stringToInt(right_now.substring(right_now.indexOf(":")+1, -1));
    int right_now_hour = stringToInt(right_now.substring(0, right_now.indexOf(":")));
  
    
    if (right_now_hour >= CaseTwo[1].system_time_h && ((right_now_minute - CaseTwo[1].system_time_m) >= 0) && (right_now_minute >= CaseTwo[1].system_time_m)){
      Serial.println(" ");
      Serial.println("WATER (CLOCK) IS MET TIME REQUIREMENTS");
      if (water_snap_workC){
        water_start_time_h = right_now_hour;
        water_start_time_m = right_now_minute;
        
        water_snap_workC = false;
        water_track_workC = true;
        
        Serial.println(" ");
        Serial.println("(!!!)Started tracking! (clock)");
        Serial1.println("c");
        is_working_water = true;
      }
      // tracking time
      if (compareTimes(water_start_time_h, water_start_time_m, right_now_hour, right_now_minute, CaseTwo[1].system_dur1) && water_track_workC){
        Serial.println(" ");
        Serial.println("(!!!)Completed tracking by min of duration! (clock)");
        Serial1.println("d");
        is_working_water = false;
        if (CaseTwo[1].system_rep){
          water_snap_workC = true;
          water_track_workC = false;        
        }
        else{
          water_snap_workC = true;
          water_track_workC = false; 
          CaseTwo[1].set_value(8, false);
          
          is_water_set = false;
          water_button_state = false;
          events.send("Refresh the page","refresher",millis());
        }  
      }
    }
  
  makeSnapShot(9, "v,"+String(int(light_snap_workC))+","+String(int(light_track_workC))+","+String(int(light_snap_workI))+","+String(int(light_track_workI))+","+String(int(light_snap_pauseI))+","+String(int(light_track_pauseI))+","+String(int(water_snap_workC))+","+String(int(water_track_workC))+","+String(int(water_snap_workI))+","+String(int(water_track_workI))+","+String(int(water_snap_pauseI))+","+String(int(water_track_pauseI))+",");    
  }
  
  else if (CaseTwo[1].is_inter_set){
    Serial.println(" ");
    Serial.println("INTERVAL WATER IS SET");
        
    String right_now = DateTimeStamp.substring(DateTimeStamp.indexOf(" ")+1, -1);
    int right_now_minute = stringToInt(right_now.substring(right_now.indexOf(":")+1, -1));
    int right_now_hour = stringToInt(right_now.substring(0, right_now.indexOf(":")));
    
    if (water_snap_workI){
      water_start_time_h = right_now_hour;
      water_start_time_m = right_now_minute;
      
      water_snap_workI = false;
      water_track_workI = true;
    
      Serial.println(" ");
      Serial.println("(!!!)Started tracking! (interval)");
      Serial1.println("c");
      is_working_water = true;
    }
    // tracking time
    if (water_track_workI){
      if (compareTimes(water_start_time_h, water_start_time_m, right_now_hour, right_now_minute, CaseTwo[1].system_dur2)){
        
        Serial.println(" ");
        Serial.println("(!!!)Completed tracking by min of duration! (interval)");
        Serial1.println("d");
        is_working_water = false;
        water_track_workI = false;
        water_snap_pauseI = true;          
      }
    }
    else if (water_snap_pauseI){
      water_start_time_h = right_now_hour;
      water_start_time_m = right_now_minute;
      
      Serial.println(" ");
      Serial.println("(!!!)Started pausing! (interval)");
      water_snap_pauseI = false; 
      water_track_pauseI = true;     
    }
    // tracking pause
    else if(water_track_pauseI){
      if (compareTimes(water_start_time_h, water_start_time_m, right_now_hour, right_now_minute, CaseTwo[1].system_pause)){
        
        Serial.println(" ");
        Serial.println("(!!!)Completed tracking by min of pause! (interval)");
        water_snap_workI = true;
        water_track_pauseI = false;  
      }    
    }

  makeSnapShot(9, "v,"+String(int(light_snap_workC))+","+String(int(light_track_workC))+","+String(int(light_snap_workI))+","+String(int(light_track_workI))+","+String(int(light_snap_pauseI))+","+String(int(light_track_pauseI))+","+String(int(water_snap_workC))+","+String(int(water_track_workC))+","+String(int(water_snap_workI))+","+String(int(water_track_workI))+","+String(int(water_snap_pauseI))+","+String(int(water_track_pauseI))+",");
  };

}




void setup() {
  Serial.begin(baud, SERIAL_8N1);
  Serial1.begin(baud, SERIAL_8N1, RXD2, TXD2);  // От и К Arduino
  
  pinMode(ONBOARD_LED,OUTPUT);

  // Инициализация Модуля SD Памяти 
  init_sd_card();
  prepare_main_files();

  
  // Инициализация Модуля Реального Времени
  initHardTimeModule();

  // Инициализация LCD дисплея
  lcd.init();
  lcd.backlight();

  // Инициализация WIFI
  initWiFi();
  
  delay(1000);



  // Главная страница
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
      request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(401);
  });
  
  // Страница настроек
  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", settings_html, processor);
  });

  // Задание значения интервала обновления показании датчиков
  server.on("/getupd", HTTP_GET, [](AsyncWebServerRequest *request){
    String new_update_span;
    String new_brightness_value;
    
    if (request->hasParam(TIME_PARAM_INPUT)) {
        new_update_span = request->getParam(TIME_PARAM_INPUT)->value();
        if (new_update_span != ""){
          Serial.println("TIME HAS VALUE");
          new_update_span = stringToInt(new_update_span)*1000;
          timerDelay = stringToInt(new_update_span);
        }
    }
    if (request->hasParam(LED_BRIGHTNESS_INPUT)) {
        new_brightness_value = request->getParam(LED_BRIGHTNESS_INPUT)->value();
        if (new_brightness_value != ""){
          Serial1.println(new_brightness_value); // Setting BRIGHTNESS OF LED
          Serial.println(" "); 
          Serial.println("BRIGHTNESS HAS VALUE");
          Serial.println(new_brightness_value);
          new_brightness_value = stringToInt(new_brightness_value);
          led_brightness = stringToInt(new_brightness_value);
        }
    }
    
    makeSnapShot(7, "g,"+String(timerDelay)+","+String(led_brightness)+",");     
    request->send_P(200, "text/html", settings_html, processor);
  });



  
  // Задание значения для СИСТЕМЫ ОСВЕЩЕНИЯ  -- ЗАДАТЧИК
  server.on("/getlight", HTTP_GET, [](AsyncWebServerRequest *request){
    String light_message_time;
    String light_message_duration1;
    String light_message_duration2;
    String light_message_repeat;
    String light_message_pause;
    
      
    // Контроль ОСВЕЩЕНИЯ
    if (request->hasParam(LIGHT_PARAM_INPUT1) && request->hasParam(LIGHT_PARAM_INPUT2)) {
        light_message_time = request->getParam(LIGHT_PARAM_INPUT1)->value();
        light_message_duration1 = request->getParam(LIGHT_PARAM_INPUT2)->value();
           
        if (light_message_time != "" && light_message_duration1 != ""){
          
          if (!light_button_state && !is_light_set){
//            Serial1.println('L');  
          }
          
          if (getFeedBack()){
          
            // HAVING SET VALUE FOR LIGHTENING
            if (request->hasParam(LIGHT_PARAM_INPUT4)){
              light_repeat = true;
              light_set_value_s = "Начало с: " + light_message_time + " прод: " + light_message_duration1 + " мин" + " (пов.)";
            }
            else{
              light_repeat = false;
              light_set_value_s = "Начало с: " + light_message_time + " прод: " + light_message_duration1 + " мин";
            }
                  
            if (!light_button_state){
              light_button_state = true;
            }
            if (!is_light_set){
              is_light_set = true;
            }
          }
          CaseTwo[0].set_value(1, stringToInt(light_message_time.substring(0, light_message_time.indexOf(":"))));
          CaseTwo[0].set_value(2, stringToInt(light_message_time.substring(-1, light_message_time.indexOf(":")+1)));
          CaseTwo[0].set_value(3, stringToInt(light_message_duration1));
          CaseTwo[0].set_value(4, light_repeat);
            
          CaseTwo[0].set_value(9, false);
          CaseTwo[0].set_value(10, false);
          CaseTwo[0].set_value(8, true);

          makeSnapShot(5, "c,"+light_message_time.substring(0, light_message_time.indexOf(":"))+","+light_message_time.substring(-1, light_message_time.indexOf(":")+1)+","+light_message_duration1+","+String(int(light_repeat))+","+String(CaseTwo[0].get_value(8))+",");            
        }   
    }
    else if (request->hasParam(LIGHT_PARAM_INPUT5) && request->hasParam(LIGHT_PARAM_INPUT6)) {
        light_message_duration2 = request->getParam(LIGHT_PARAM_INPUT5)->value();
        light_message_pause = request->getParam(LIGHT_PARAM_INPUT6)->value();
     
        if (light_message_pause != "" && light_message_duration2 != ""){
          
          if (!light_button_state && !is_light_set){
//            Serial1.println('L');  
          }
          if (getFeedBack()){
            // HAVING SET VALUE FOR LIGHTENING
            light_set_value_s = "В течение: " + light_message_duration2 + " с паузой в: " + light_message_pause + " мин";
          }
          
          if (!light_button_state){
            light_button_state = true;
          }
          if (!is_light_set){
            is_light_set = true;
          }
        }
        CaseTwo[0].set_value(5, stringToInt(light_message_duration2));
        CaseTwo[0].set_value(6, stringToInt(light_message_pause));
          
        CaseTwo[0].set_value(8, false);
        CaseTwo[0].set_value(10, false);
        CaseTwo[0].set_value(9, true);

        makeSnapShot(5, "i,"+light_message_duration2+","+light_message_pause+","+String(CaseTwo[0].get_value(9))+",");        
    };
        request->send_P(200, "text/html", index_html, processor);
  });


  // Задание значения для СИСТЕМЫ ОСВЕЩЕНИЯ -- ВКЛ/ВЫКЛ
  server.on("/getlighttog", HTTP_GET, [](AsyncWebServerRequest *request){
    String light_message_toggle;
              
    if (request->hasParam(LIGHT_PARAM_INPUT3)) {
        light_message_toggle = request->getParam(LIGHT_PARAM_INPUT3)->value();
    }

//    Serial1.println('L');
      
    if (getFeedBack()){
    
      if(light_message_toggle = "toggle-light"){
        if (light_button_state){
            light_button_state = false;
            is_light_set = false;
        }
        else{
          light_button_state = true;
        }
      }
    }
    CaseTwo[0].set_value(7, int(light_button_state));
    
    CaseTwo[0].set_value(8, false);
    CaseTwo[0].set_value(9, false);
    CaseTwo[0].set_value(10, light_button_state);

    makeSnapShot(5, "k,"+String(int(light_button_state))+","+String(CaseTwo[0].get_value(10))+",");     
    request->send_P(200, "text/html", index_html, processor);
  });





  // Задание значения для СИСТЕМЫ ПОЛИВА  -- ЗАДАТЧИК
  server.on("/getwater", HTTP_GET, [](AsyncWebServerRequest *request){
    String water_message_time;
    String water_message_duration1;
    String water_message_duration2;
    String water_message_repeat;
    String water_message_pause;

      
    // Контроль ПОЛИВА
    if (request->hasParam(WATER_PARAM_INPUT1) && request->hasParam(WATER_PARAM_INPUT2)) {
        water_message_time = request->getParam(WATER_PARAM_INPUT1)->value();
        water_message_duration1 = request->getParam(WATER_PARAM_INPUT2)->value();
           
        if (water_message_time != "" && water_message_duration1 != ""){
          
          if (!water_button_state && !is_water_set){
//            Serial1.println('W');  
          }
          
          if (getFeedBack()){
          
            // HAVING SET VALUE FOR WATERING
            if (request->hasParam(WATER_PARAM_INPUT4)){
              water_repeat = true;
              water_set_value_s = "Начало с: " + water_message_time + " прод: " + water_message_duration1 + " мин" + " (пов.)";
            }
            else{
              water_repeat = false;
              water_set_value_s = "Начало с: " + water_message_time + " прод: " + water_message_duration1 + " мин";
            }
                  
            if (!water_button_state){
              water_button_state = true;
            }
            if (!is_water_set){
              is_water_set = true;
            }
          }
          CaseTwo[1].set_value(1, stringToInt(water_message_time.substring(0, water_message_time.indexOf(":"))));
          CaseTwo[1].set_value(2, stringToInt(water_message_time.substring(-1, water_message_time.indexOf(":")+1)));
          CaseTwo[1].set_value(3, stringToInt(water_message_duration1));
          CaseTwo[1].set_value(4, water_repeat);
            
          CaseTwo[1].set_value(9, false);
          CaseTwo[1].set_value(10, false);
          CaseTwo[1].set_value(8, true);

          makeSnapShot(6, "c,"+water_message_time.substring(0, water_message_time.indexOf(":"))+","+water_message_time.substring(-1, water_message_time.indexOf(":")+1)+","+water_message_duration1+","+String(int(water_repeat))+","+String(CaseTwo[1].get_value(8))+",");
        }   
    }
    else if (request->hasParam(WATER_PARAM_INPUT5) && request->hasParam(WATER_PARAM_INPUT6)){
        water_message_pause = request->getParam(WATER_PARAM_INPUT6)->value();
        water_message_duration2 = request->getParam(WATER_PARAM_INPUT5)->value();
        
        if (water_message_pause != "" && water_message_duration2 != ""){
          
          if (!water_button_state && !is_water_set){
//            Serial1.println('W');  
          }
          if (getFeedBack()){
            
            // HAVING SET VALUE FOR WATERING
            water_set_value_s = "В течение: " + water_message_duration2 + " с паузой в: " + water_message_pause + " мин";
            if (!water_button_state){
              water_button_state = true;
            }
            if (!is_water_set){
              is_water_set = true;
            }            
          }             
        }
        CaseTwo[1].set_value(5, stringToInt(water_message_duration2));
        CaseTwo[1].set_value(6, stringToInt(water_message_pause));
          
        CaseTwo[1].set_value(8, false);
        CaseTwo[1].set_value(10, false);
        CaseTwo[1].set_value(9, true);

        makeSnapShot(6, "i,"+water_message_duration2+","+water_message_pause+","+String(CaseTwo[1].get_value(9))+",");    
    }
        request->send_P(200, "text/html", index_html, processor);
  });


  // Задание значения для СИСТЕМЫ ПОЛИВА -- ВКЛ/ВЫКЛ
  server.on("/getwatertog", HTTP_GET, [](AsyncWebServerRequest *request){
    String water_message_toggle;
              
    if (request->hasParam(WATER_PARAM_INPUT3)) {
        water_message_toggle = request->getParam(WATER_PARAM_INPUT3)->value();
    }

//    Serial1.println('W');
      
    if (getFeedBack()){
    
      if(water_message_toggle = "toggle-water"){
        if (water_button_state){
            water_button_state = false;
            is_water_set = false;
        }
        else{
          water_button_state = true;
        }
      }
    }
    CaseTwo[1].set_value(7, int(water_button_state));
    
    CaseTwo[1].set_value(8, false);
    CaseTwo[1].set_value(9, false);
    CaseTwo[1].set_value(10, water_button_state);

    makeSnapShot(6, "k,"+String(int(water_button_state))+","+String(CaseTwo[1].get_value(10))+",");     
    request->send_P(200, "text/html", index_html, processor);
  });





  // Задание значения для СИСТЕМЫ ОТОПЛЕНИЯ ВОЗДУХА
  server.on("/gettemp", HTTP_GET, [](AsyncWebServerRequest *request){
      String temp_message;
      
      // Контроль ТЕМПЕРАТУРЫ
      if (request->hasParam(TEMP_PARAM_INPUT1)) {
        temp_message = request->getParam(TEMP_PARAM_INPUT1)->value();
        
        if (temp_message != ""){
    
          // HAVING SET VALUE ON TEMPERATURE
          temp_set_value_s = temp_message;
          temp_set_value_f = stringToFloat(temp_message);
          
          if (!temp_button_state && !is_temp_set){
//            Serial1.println('T');  
          }
           
          if (getFeedBack()){
            if (!temp_button_state){
              temp_button_state = true;
            }
            if (!is_temp_set){
              is_temp_set = true;  
            }    
          }
        }
      CaseOne[0].set_value(1, stringToInt(temp_message));
      CaseOne[0].set_value(3, true);
      
      makeSnapShot(0, "s,"+temp_message+","+"1,");  
      }
          
      else if (request->hasParam(TEMP_PARAM_INPUT2)) {
          temp_message = request->getParam(TEMP_PARAM_INPUT2)->value();

//          Serial1.println('T');
           
          if (getFeedBack()){
          
            if (temp_message == "toggle-temp"){     
              if (temp_button_state){
                temp_button_state = false;
                is_temp_set = false;
              }
              else{
                temp_button_state = true;
              }
            }      
          temp_set_value_f = 0.00;
          }
   
      CaseOne[0].set_value(2, int(temp_button_state));
      CaseOne[0].set_value(4, temp_button_state);
      CaseOne[0].set_value(3, is_temp_set);
            
      makeSnapShot(0, "o,"+String(int(temp_button_state))+","+String(int(temp_button_state))+",");   
      };
      
      request->send_P(200, "text/html", index_html, processor);
  });

  
  // Задание значения для СИСТЕМЫ УВЛАЖНЕНИЯ ВОЗДУХА
  server.on("/gethum", HTTP_GET, [](AsyncWebServerRequest *request){
      String hum_message;
      
      // Контроль ВЛАЖНОСТИ
      if (request->hasParam(HUM_PARAM_INPUT1)) {
        hum_message = request->getParam(HUM_PARAM_INPUT1)->value();
        
        if (hum_message != ""){
    
          // HAVING SET VALUE ON HUMIDITY
          hum_set_value_s = hum_message;
          hum_set_value_f = stringToFloat(hum_message);
          
          if (!hum_button_state && !is_hum_set){
//            Serial1.println('H');                   // !!! NEW VALUE FOR ARDUINO !!!
          }
           
          if (getFeedBack()){
            if (!hum_button_state){
              hum_button_state = true;
            }
            if (!is_hum_set){
              is_hum_set = true;  
            }    
          }
        }
      CaseOne[1].set_value(1, stringToInt(hum_message));
      CaseOne[1].set_value(3, true);
      
      makeSnapShot(1, "s,"+hum_message+","+"1,");        
      }
          
      else if (request->hasParam(HUM_PARAM_INPUT2)) {
          hum_message = request->getParam(HUM_PARAM_INPUT2)->value();

//          Serial1.println('H');
           
          if (getFeedBack()){
          
            if (hum_message == "toggle-hum"){     
              if (hum_button_state){
                hum_button_state = false;
                is_hum_set = false;
              }
              else{
                hum_button_state = true;
              }
            }      
          hum_set_value_f = 0.00;
          }
          
      CaseOne[1].set_value(2, int(hum_button_state));
      CaseOne[1].set_value(4, hum_button_state);
      CaseOne[1].set_value(3, is_hum_set);
      
      makeSnapShot(1, "o,"+String(int(hum_button_state))+","+String(int(hum_button_state))+",");      
      };
      
      request->send_P(200, "text/html", index_html, processor);
  });

  
  // Задание значения для СИСТЕМЫ СЛЕЖЕНИЯ ЗА СОДЕРЖАНИЕМ УГЛЕКИСЛОГО ГАЗА
  server.on("/getcarbon", HTTP_GET, [](AsyncWebServerRequest *request){
      String carbon_message;
      
      // Контроль CO2
      if (request->hasParam(CARBON_PARAM_INPUT1)) {
        carbon_message = request->getParam(CARBON_PARAM_INPUT1)->value();
        
        if (carbon_message != ""){
    
          // HAVING SET VALUE ON CO2
          carbon_set_value_s = carbon_message;
          carbon_set_value_f = stringToFloat(carbon_message);
          
          if (!carbon_button_state && !is_carbon_set){
//            Serial1.println('C');                   // !!! NEW VALUE FOR ARDUINO !!!
          }
           
          if (getFeedBack()){
            if (!carbon_button_state){
              carbon_button_state = true;
            }
            if (!is_carbon_set){
              is_carbon_set = true;  
            }    
          }
        }
      CaseOne[2].set_value(1, stringToInt(carbon_message));
      CaseOne[2].set_value(3, true);
      
      makeSnapShot(2, "s,"+carbon_message+","+"1,");                  
      }
          
      else if (request->hasParam(CARBON_PARAM_INPUT2)) {
          carbon_message = request->getParam(CARBON_PARAM_INPUT2)->value();

//          Serial1.println('C');
           
          if (getFeedBack()){
          
            if (carbon_message == "toggle-carbon"){     
              if (carbon_button_state){
                carbon_button_state = false;
                is_carbon_set = false;
              }
              else{
                carbon_button_state = true;
              }
            }      
          carbon_set_value_f = 0.00;
          }
      
      CaseOne[2].set_value(2, int(carbon_button_state));
      CaseOne[2].set_value(4, carbon_button_state);
      CaseOne[2].set_value(3, is_carbon_set);
      
      makeSnapShot(2, "o,"+String(int(carbon_button_state))+","+String(int(carbon_button_state))+",");          
      };
      
      request->send_P(200, "text/html", index_html, processor);
  });

  
  // Задание значения для СИСТЕМЫ СЛЕЖЕНИЯ ЗА ТЕМПЕРАТУРОЙ ВОДЫ В БАКЕ
  server.on("/getwatertemp", HTTP_GET, [](AsyncWebServerRequest *request){
      String water_temp_message;
      
      // Контроль ТЕМПЕРАТУРЫ ВОДЫ
      if (request->hasParam(WATER_TEMP_PARAM_INPUT1)) {
        water_temp_message = request->getParam(WATER_TEMP_PARAM_INPUT1)->value();
        
        if (water_temp_message != ""){
    
          // HAVING SET VALUE ON WATER TEMPERATURE
          water_temp_set_value_s = water_temp_message;
          water_temp_set_value_f = stringToFloat(water_temp_message);
          
          if (!water_temp_button_state && !is_water_temp_set){
//            Serial1.println('X');                   // !!! NEW VALUE FOR ARDUINO !!!
          }
           
          if (getFeedBack()){
            if (!water_temp_button_state){
              water_temp_button_state = true;
            }
            if (!is_water_temp_set){
              is_water_temp_set = true;  
            }    
          }
        }  
      CaseOne[3].set_value(1, stringToInt(water_temp_message));
      CaseOne[3].set_value(3, true);
      
      makeSnapShot(3, "s,"+water_temp_message+","+"1,");                        
      }
          
      else if (request->hasParam(WATER_TEMP_PARAM_INPUT2)) {
          water_temp_message = request->getParam(WATER_TEMP_PARAM_INPUT2)->value();

//          Serial1.println('X');
           
          if (getFeedBack()){
          
            if (water_temp_message == "toggle-water-temp"){     
              if (water_temp_button_state){
                water_temp_button_state = false;
                is_water_temp_set = false;
              }
              else{
                water_temp_button_state = true;
              }
            }      
          water_temp_set_value_f = 0.00;
          }
      CaseOne[3].set_value(2, int(water_temp_button_state));
      CaseOne[3].set_value(4, water_temp_button_state);
      CaseOne[3].set_value(3, is_water_temp_set);
      
      makeSnapShot(3, "o,"+String(int(water_temp_button_state))+","+String(int(water_temp_button_state))+",");                
      };
      request->send_P(200, "text/html", index_html, processor);
  });

  
  // Задание значения для СИСТЕМЫ СЛЕЖЕНИЯ ЗА УРОВНЕМ ВОДЫ В БАКЕ
  server.on("/getwaterlevel", HTTP_GET, [](AsyncWebServerRequest *request){
      String water_level_message;
      
      // Контроль УРОВНЯ ВОДЫ
      if (request->hasParam(WATER_LEVEL_PARAM_INPUT1)) {
        water_level_message = request->getParam(WATER_LEVEL_PARAM_INPUT1)->value();
        
        if (water_level_message != ""){
    
          // HAVING SET VALUE ON WATER TEMPERATURE
          water_level_set_value_s = water_level_message;
          water_level_set_value_f = stringToFloat(water_level_message);
          
          if (!water_level_button_state && !is_water_level_set){
//            Serial1.println('Y');                   // !!! NEW VALUE FOR ARDUINO !!!
          }
           
          if (getFeedBack()){
            if (!water_level_button_state){
              water_level_button_state = true;
            }
            if (!is_water_level_set){
              is_water_level_set = true;  
            }    
          }
        }  
      CaseOne[4].set_value(1, stringToInt(water_level_message));
      CaseOne[4].set_value(3, true);
      
      makeSnapShot(4, "s,"+water_level_message+","+"1,");                            
      }
          
      else if (request->hasParam(WATER_LEVEL_PARAM_INPUT2)) {
          water_level_message = request->getParam(WATER_LEVEL_PARAM_INPUT2)->value();

//          Serial1.println('Y');
           
          if (getFeedBack()){
          
            if (water_level_message == "toggle-water-level"){     
              if (water_level_button_state){
                water_level_button_state = false;
                is_water_level_set = false;
              }
              else{
                water_level_button_state = true;
              }
            }      
          water_level_set_value_f = 0.00;
          }
      
      CaseOne[4].set_value(2, int(water_level_button_state));
      CaseOne[4].set_value(4, water_level_button_state);
      CaseOne[4].set_value(3, is_water_level_set);
      
      makeSnapShot(4, "o,"+String(int(water_level_button_state))+","+String(int(water_level_button_state))+",");                      
      };
      
      request->send_P(200, "text/html", index_html, processor);
  });

  

  // Задание значения для СИСТЕМЫ ВОЗДУХООБМЕНА !!! НЕ ИСПОЛЬЗУЕТСЯ (ПОКА) !!!
//  server.on("/getfan", HTTP_GET, [](AsyncWebServerRequest *request){
//      String fan_message; 
//      // Контроль ВЕНТИЛЯТОРА          
//      if (request->hasParam(FAN_PARAM_INPUT)) {
//          fan_message = request->getParam(FAN_PARAM_INPUT)->value();
//      }
//      Serial1.println('F');
//      if (getFeedBack()){     
//        if (fan_message == "toggle-fan"){     
//          if (fan_button_state){
//            fan_button_state = false;
//          }
//          else{
//            fan_button_state = true;
//          }
//        }     
//      }
//      request->send_P(200, "text/html", index_html, processor);
//  });



    
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
//      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.onNotFound(notFound);
  server.begin();



  if (gui_control_mode == "web-based"){
    Serial.println(" ");
    Serial.println("Everything is OK");
  }
  else{
    update_display();    
  }

}


void loop() {
    
    if (gui_control_mode == "manual"){ 
    
      // обязательная функция отработки. Должна постоянно опрашиваться
      enc1.tick();
      
      if (enc1.isRight()){                  // если был поворот направо
    //    Serial.println("Right");
        update_place(1);
      };         
      
      if (enc1.isLeft()){                   // если был поворот налево
    //    Serial.println("Left");
        update_place(-1);
      };
      
      if (enc1.isRightH()){
    //    Serial.println("Right holded");
        update_system(1);
       
      };                                    // если было удержание + поворот направо
    
      if (enc1.isLeftH()){
    //    Serial.println("Left holded");
        update_system(-1);
        
      };                                    // если было удержание + поворот налева
      
      if (enc1.isPress()){
    //    Serial.println("Press");            // нажатие на кнопку (+ дебаунс)
      };
      
      if (enc1.isRelease()){
    //    Serial.println("Release");          // отпускание кнопки (+ дебаунс)
        update_level();
    //    track_cursor();
        toggle_editing_mode();
        update_display();
      };
      
      if (enc1.isHolded()){
    //    Serial.println("Holded");           // если была удержана и энк не поворачивался
        set_current_system();
        update_display();
      };
      
    //  if (enc1.isHold()){
    //    Serial.println("Hold");            // возвращает состояние кнопки
    //  };
    
    
      if (enc1.isTurn()) {                   // если был совершён поворот (индикатор поворота в любую сторону)
        update_display();
    //    track_cursor(); 
      }


      if ((millis() - lastTime) > timerDelay) {
        
        // Read the sersors reading
        getSensorsReadings();
        blinkBuildLED();
        update_display();
        // Update screen values
  
  
        // TRYING TO REACH WIFI SIGNAL
        int networks = WiFi.scanNetworks();
        if (networks){
          for (int i = 0; i < networks; i++){
            if (WiFi.SSID(i) == main_ssid || WiFi.SSID(i) == other_ssid || WiFi.SSID(i) == another_ssid){
              initWiFi();  
            }
          }
        }

        TrackSystems();
        makeSnapShot(8, "r,"+String(int(is_working_temperature))+","+String(int(is_working_humidity))+","+String(int(is_working_carbon))+","+String(int(is_working_water_temp))+","+String(int(is_working_water_level))+","+String(int(is_working_light))+","+String(int(is_working_water))+",");
        lastTime = millis();   
      }
  
  }
  else{ // WEB BASED MODE 

    // Сверка значении датчиков и обновление страницы каждые timerDelay секунд (настраиваемые).
    if ((millis() - lastTime) > timerDelay) {
      
      // Получение данных о времени и значении с датчиковы
      getWiFiDateTime();
      getSensorsReadings();
      
      dummy_temperature = temperature;
      dummy_humidity = humidity;
      dummy_light = light;
  
      // Вывод IP адреса и времени на LCD дисплей  
      display_wifi_info();
      lcd.setCursor(1, 3);  
      lcd.print(DateTimeStamp);
  
  
  //  ********************************  ДЕБАГГИНГ  ***********************************
  
  //    Serial.println(temp_set_value_f);
  //    if (temp_set_value_f != 0){
  //      if (temp_set_value_f <= temperature){
  //       
  //        Serial1.println('b');
  //        temp_button_state = false;
  //        is_temp_set = false;
  //        events.send("Refresh the page","refresher",millis());        
  //      }
  //      else{
  //        Serial1.println('d');
  //      }
  //    }
      
  //  ********************************  ДЕБАГГИНГ  ***********************************
  
      
      // Отправка и Обновление значении на Веб-странице
      events.send("ping",NULL,millis());    
      events.send(String(DateTimeStamp).c_str(),"datetime",millis());
      events.send(String(temperature).c_str(),"temperature",millis());
      events.send(String(humidity).c_str(),"humidity",millis());
      events.send(String(light).c_str(),"light",millis());
      
      TrackSystems();
      makeSnapShot(8, "r,"+String(int(is_working_temperature))+","+String(int(is_working_humidity))+","+String(int(is_working_carbon))+","+String(int(is_working_water_temp))+","+String(int(is_working_water_level))+","+String(int(is_working_light))+","+String(int(is_working_water))+",");
      lastTime = millis();
    }    
  }

    // ***** MAIN CONTROL FORM ESP32 AND COMMANDS SENDING TO ARDUINO MEGA  *****

}
