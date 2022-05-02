#include <LiquidCrystal_I2C.h>
#include "GyverEncoder.h"
#include "RTClib.h"


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

Encoder enc1(CLK, DT, SW, true);
LiquidCrystal_I2C lcd(0x27, 20, 4);

RTC_DS3231 rtc;

int baud = 9600;

// Интервал обновления показании датчиков
unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;    // КАЖДЫЕ 30 секунд

// Переменные для хранения и обработки значении времени для Веб-страницы
String formattedDate;
String dayStamp;
String timeStamp;
String DateTimeStamp;
char datestring[20];

// **** Main system variables ****

int temperature = 0;
int humidity = 0;
int carbon = 0;
int water_temperature = 0;
int water_level = 0;
int water = 0;
int light = 0;

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


// **** Main Structures for storing Data **** 

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
  uint8_t system_num;
  String system_name;
} L1systems;
const L1systems L1system_titles[] {
    {0, "Temperature"},
    {1, "CO2 Content"},
    {2, "Humidity"},
    {3, "Water Temp."},
    {4, "Water Level"},
    {5, "Watering"},
    {6, "Lightening"},
};

typedef struct { 
  uint8_t system_num;
  String system_name;
} L2systems;
const L2systems L2system_titles[] {
    {0, "Temp Set Mode"},
    {1, "CO2 Set Mode"},
    {2, "Humid Set Mode"},
    {3, "W.Temp. Set Mode"},
    {4, "W.Level Set Mode"},
    {5, "Water Clock"},
    {6, "Light Clock"},
};

typedef struct { 
  uint8_t system_num;
  String system_name;
} L3systems;
const L3systems L3system_titles[] {
    {0, "Temp On-Off"},
    {1, "CO2 On-Off"},
    {2, "Humid On-Off"},
    {3, "W.Temp. On-Off"},
    {4, "W.Level On-Off"},
    {5, "Water Inter."},
    {6, "Light Inter."},
};

typedef struct { 
  uint8_t system_num;
  String system_name;
} L4systems;
const L4systems L4system_titles[] {
    {0, "Water On-Off"},
    {1, "Light On-Off"},
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

    Serial1.write('S');
//    delay(500);
    
    if (Serial1.available() > 0){
      temperature = Serial1.read();
      humidity = Serial1.read();
      light = Serial1.read();
      int garbage1 = Serial1.read();
      int garbage2 = Serial1.read();
      int garbage3 = Serial1.read();

      // UPDATING SENSORS VALUES FROM ARDUINO MEGA
      CaseOne[0].set_value(0, temperature);
      CaseOne[2].set_value(0, humidity);
      CaseTwo[1].set_value(0, light);
  
      Serial.println();
      Serial.println("Recieved values: ");
      Serial.println();
      Serial.print("temperature: ");
      Serial.println(temperature);
      Serial.print("humidity: ");
      Serial.println(humidity);    
      Serial.print("light: ");
      Serial.println(light);
    }
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


int value_changer_with_restrictionsONE(int where, int low_end, int hight_end){
  
  int starter_value = CaseOne[systems_pointer].get_value(levels_pointer);
  bool is_system_set = CaseOne[systems_pointer].get_value(3);
  
  Serial.println("Starter Value: " + String(starter_value));
  
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
    CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(7, value_changer_with_restrictionsTWO(where, 0, 1, 7));
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
    if (where < 0 && systems_pointer < countof(L1system_titles)-1){
      systems_pointer++;
    }
    else if (where > 0 && systems_pointer > 0){
      systems_pointer--;
    };
  
    if (systems_pointer < countof(L1system_titles)-2){
      current_case = 0;
    }
    else{
      current_case = 1;
    }
  }
}

void update_level(){
  if (places_pointer == 0){
    if (levels_pointer < case_deepness[current_case].case_level_deepness){
      levels_pointer++;  
    }
    else{
      levels_pointer = 0;
    }  
  }
}

void track_cursor(){
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
  lcd.setCursor(options[places_pointer].col, options[places_pointer].row);
  lcd.print(update_cursor_type());  
}

char system_set_icon(){
  if (systems_pointer < 5){
    bool is_set = CaseOne[systems_pointer].get_value(levels_pointer+2);
    if (is_set){
      return '$';  
    }    
  }
  else{
    bool is_set = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(levels_pointer+7);
    if (is_set){
      return '$';
    }
  }
  return ' ';
}

String current_title(){
  
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
//  levels_pointer, systems_pointer, places_pointer
// options[0].row.col

  // LEVEL 1
  if (levels_pointer == 0){
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
  Serial.println("SET ON System: " + String(systems_pointer) + " Level: " + String(levels_pointer));
  // CASES 1
  if (systems_pointer < 5){
    
    if (levels_pointer == 1){
      bool current_state = CaseOne[systems_pointer].get_value(3);
      bool other_state = CaseOne[systems_pointer].get_value(4);
      
      if (current_state){
        CaseOne[systems_pointer].set_value(3, false);
      }
      else if (!other_state){
        CaseOne[systems_pointer].set_value(3, true);
      }
    }
    else if (levels_pointer == 2){
      bool current_state = CaseOne[systems_pointer].get_value(4);
      bool other_state = CaseOne[systems_pointer].get_value(3);
      
      if (current_state){
        CaseOne[systems_pointer].set_value(4, false);
      }
      else if (!other_state){
        CaseOne[systems_pointer].set_value(4, true);
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
      }
      else if (!other_state && !another_state){
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(8, true);
      }  
    }
    else if (levels_pointer == 2){
      bool current_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(9);
      bool other_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(8);
      bool another_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(10);
      
      if (current_state){
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(9, false);
      }
      else if (!other_state && !another_state){
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(9, true);
      }      
    }
    else if (levels_pointer == 3){
      bool current_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(10);
      bool other_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(8);
      bool another_state = CaseTwo[systems_pointer-(countof(L1system_titles)-2)].get_value(9);
      
      if (current_state){
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(10, false);
      }
      else if (!other_state && !another_state){
        CaseTwo[systems_pointer-(countof(L1system_titles)-2)].set_value(10, true);
      }      
    }    
  
  }

}



void setup() {
  pinMode(ONBOARD_LED,OUTPUT);
  
  // Инициализация LCD дисплея
  lcd.init();
  lcd.backlight();

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
//  rtc.adjust(DateTime(__DATE__, __TIME__));
  
  Serial.begin(baud, SERIAL_8N1);
  Serial1.begin(baud, SERIAL_8N1, RXD2, TXD2);  // От и К Arduino
  update_display();
}


void loop() {
  
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
    
    lastTime = millis();
  }
}
