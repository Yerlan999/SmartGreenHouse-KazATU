#include <LiquidCrystal_I2C.h>
#include "GyverEncoder.h"

/* 
  CLK == S1
  DT == S2
  SW == Key 
*/

#define CLK 33 // S1
#define DT 25  // S2
#define SW 32  // Key


Encoder enc1(CLK, DT, SW, true);
LiquidCrystal_I2C lcd(0x27, 20, 4);


// **** Main system variables ****

int temperature = 31;
int humidity = 50;
int carbon = 10;
int water_temperature = 36;
int water_level = 5;
int water = 1;
int light = 22;

bool temperature_set = false;
bool humidity_set = false;
bool carbon_set = false;
bool water_temp_set = false;
bool water_level_set = false;
bool watering_set = false;
bool lighting_set = false;

int temperature_set_value = 0;
int humidity_set_value = 0;
int carbon_set_value = 0;
int water_temp_set_value = 0;
int water_level_set_value = 0;

int watering_time_hour = 0;
int watering_time_minute = 0;
int watering_duration = 0;
bool watering_time_repeat = false;
bool watering_state = false;

int lighting_time_hour = 0;
int lighting_time_minute = 0;
int lighting_duration = 0;
bool lighting_time_repeat = false;
bool lighting_state = false;


bool temperature_state = false;
bool humidity_state = false;
bool carbon_state = false;
bool water_temp_state = false;
bool water_level_state = false;



// **** Main Structures for storing Data **** 

struct case_one_params{ 
  uint8_t system_num;
  int system_val;
  bool is_system_set;
  int system_set_val;
  bool system_state;

  int get_value(int which){
    if (which == 1){
      return this->system_val;
    }
    else if(which == 2){
      return int(this->is_system_set);
    }
    else if(which == 3){
      return this->system_set_val;
    }
    else if(which == 4){
      return int(this->system_state);
    }  
  }

  void set_value(int which, int new_value){
    if (which == 1){
      this->system_val = new_value;
    }
    else if(which == 2){
      this->is_system_set = bool(new_value);
    }
    else if(which == 3){
      this->system_set_val = new_value;
    }
    else if(which == 4){
      this->system_state = bool(new_value);
    }
  }
};
case_one_params CaseOne[] {
    {0, temperature, temperature_set, temperature_set_value, temperature_state},
    {1, humidity, humidity_set, humidity_set_value, humidity_state},
    {2, carbon, carbon_set, carbon_set_value, carbon_state},
    {3, water_temperature, water_temp_set, water_temp_set_value, water_temp_state},
    {4, water_level, water_level_set, water_level_set_value, water_level_state},
};


struct case_two_params{ 
  uint8_t system_num;
  int system_val;
  bool is_system_set;
  int system_time_h;
  int system_time_m;
  int system_dur;
  bool system_rep;
  bool system_state;

  int get_value(int which){
    if (which == 1){
      return this->system_val;
    }
    else if(which == 2){
      return int(this->is_system_set);
    }
    else if(which == 3){
      return this->system_time_h;
    }
    else if(which == 4){
      return this->system_time_m;
    }
    else if(which == 5){
      return this->system_dur;
    }
    else if(which == 6){
      return int(this->system_rep);
    }
    else if(which == 7){
      return int(this->system_state);
    }  
  }

  void set_value(int which, int new_value){
    if (which == 1){
      this->system_val = new_value;
    }
    else if(which == 2){
      this->is_system_set = bool(new_value);
    }
    else if(which == 3){
      this->system_time_h = new_value;
    }
    else if(which == 4){
      this->system_time_m = new_value;
    }
    else if(which == 5){
      this->system_dur = new_value;
    }
    else if(which == 6){
      this->system_rep = bool(new_value);
    }
    else if(which == 7){
      this->system_state = bool(new_value);
    }
  }
};
case_two_params CaseTwo[] {
    {5, light, lighting_set, lighting_time_hour, lighting_time_minute, lighting_duration, lighting_time_repeat, lighting_state},
    {6, water, watering_set, watering_time_hour, watering_time_minute, watering_duration, watering_time_repeat, watering_state},
};



typedef struct { 
  uint8_t place;
  uint8_t row;
  uint8_t col;
} coordinates;
const coordinates options[] {
    {0, 0, 3},
    {1, 1, 0},
    {2, 1, 11},
    {3, 2, 0},
    {4, 2, 11},
};


typedef struct { 
  uint8_t system_num;
  String system_name;
} systems;
const systems system_titles[] {
    {0, "Temperature"},
    {1, "CO2 Content"},
    {2, "Humidity"},
    {3, "Water Temp."},
    {4, "Water Level"},
    {5, "Watering"},
    {6, "Lightening"},
};


int main_places_pointer = 0; // from 0 to 4
int main_systems_pointer = 0; // from 0 to 6

int new_value_counter = 0;    // from 0 to infinity
int new_time_hour_value = 0;  // from 0 to 23
int new_time_minute_value = 0;  // from 0 to 59
int new_state_value = 0;  // from 0 to 1

bool value_is_set = false;

char hover_cursor = '>';   // for mode 0
char select_cursor = '-';  // for mode 1
char switch_cursor = '=';  // for mode 2

int editing_mode = 0;


int value_changer_with_restrictionsONE(int where, int low_end, int hight_end){
  
  int starter_value = CaseOne[main_systems_pointer].get_value(main_places_pointer+1);
  
  if (where < 0 && starter_value < hight_end){
    starter_value++;
  }
  else if (where > 0 && starter_value > low_end){
    starter_value--;
  };

  return starter_value;
}


int value_changer_with_restrictionsTWO(int where, int low_end, int hight_end, bool is_hour, bool is_min){
  
  int starter_value;
  
  if (is_hour){
    starter_value = CaseTwo[main_systems_pointer-5].get_value(3);
  }
  else if (is_min){
    starter_value = CaseTwo[main_systems_pointer-5].get_value(4);
  } 
  else{
    starter_value = CaseTwo[main_systems_pointer-5].get_value(main_places_pointer);
  }
  
  if (where < 0 && starter_value < hight_end){
    starter_value++;
  }
  else if (where > 0 && starter_value > low_end){
    starter_value--;
  };

  return starter_value;
}



void toggle_editing_mode(){
  if (main_places_pointer != 0){

    if (editing_mode == 1 && main_systems_pointer > 4 && main_places_pointer == 1){
      editing_mode = 2;
    }
    else if(editing_mode == 2 && main_systems_pointer > 4 && main_places_pointer == 1){
      editing_mode = 0;
    }
    else if(editing_mode == 0 && main_systems_pointer > 4 && main_places_pointer == 1){
      editing_mode = 1;
    }
    
    else if(editing_mode == 0 && main_systems_pointer > 4 && main_places_pointer != 1){
      editing_mode = 1;
    }
    else if(editing_mode == 1 && main_systems_pointer > 4 && main_places_pointer != 1){
      editing_mode = 0;
    }
    
    else if (editing_mode == 1 && main_systems_pointer < 5){
      editing_mode = 0;
    }
    else if (editing_mode == 0 && main_systems_pointer < 5){
      editing_mode = 1;
    }
  }
}


void update_menu(){
  if (main_systems_pointer < 5){
    lcd.setCursor(1, 1);
    lcd.print("val:" + String(CaseOne[main_systems_pointer].system_val));
    
    lcd.setCursor(12, 1);
    lcd.print("set:" + String(CaseOne[main_systems_pointer].system_set_val));
    
    lcd.setCursor(1, 2);
    lcd.print("state:" + String(CaseOne[main_systems_pointer].system_state));
    
  }
  else{
    lcd.setCursor(1, 1);
    lcd.print("time:" + String(CaseTwo[main_systems_pointer-5].system_time_h) +" " + ":" + String(CaseTwo[main_systems_pointer-5].system_time_m));
    
    lcd.setCursor(12, 1);
    lcd.print("dur:" + String(CaseTwo[main_systems_pointer-5].system_dur));

    lcd.setCursor(1, 2);
    lcd.print("rep:" + String(CaseTwo[main_systems_pointer-5].system_rep));
    
    lcd.setCursor(12, 2);
    lcd.print("state:" + String(CaseTwo[main_systems_pointer-5].system_state));
  }
}


void update_cursor(){
  lcd.setCursor(options[main_places_pointer].col, options[main_places_pointer].row);
  lcd.print(update_cursor_type());  
}

void update_title(){
  lcd.setCursor(4, 0);
  lcd.print(system_titles[main_systems_pointer].system_name);
}

void update_display(){
  lcd.clear();
  update_cursor();
  update_title();
  update_menu();
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


int poiter_stopper(){
  if (main_systems_pointer < 5 ){
    return 3;  
  }
  else{
    return 4;
  };
}



void editing_values(int where){
  Serial.println("Option of " + String(main_places_pointer) + " System of " + String(main_systems_pointer) + " with Editing Mode of " + String(editing_mode) + "Direction where: " + String(where));
  
  if (main_systems_pointer < 5){
    // Edit set values and state of Case 1
    CaseOne[main_systems_pointer].set_value(main_places_pointer+1, value_changer_with_restrictionsONE(where, 0, 23));
  }
  else{
    // Case 2
    if (main_places_pointer == 1){
      if (editing_mode == 1){
        // Edit hours
        CaseTwo[main_systems_pointer-5].set_value(3, value_changer_with_restrictionsTWO(where, 0, 23, true, false));
      }
      else{ // editing_mode = 2
        // Edit minutes
        CaseTwo[main_systems_pointer-5].set_value(4, value_changer_with_restrictionsTWO(where, 0, 59, false, true));
      }
    }
    else{
      // Edit the rest
      CaseTwo[main_systems_pointer-5].set_value(main_places_pointer, value_changer_with_restrictionsTWO(where, 0, 23, false, false));
    }
  }
}


void update_pointer(int where){
  if (editing_mode == 0){
    if (where < 0 && main_places_pointer < poiter_stopper()){
      if (main_systems_pointer < 5 && main_places_pointer == 0){
        main_places_pointer += 2;
      }
      else{
        main_places_pointer++;
      }
    }
    else if (where > 0 && main_places_pointer > 0){
      if (main_systems_pointer < 5 && main_places_pointer == 2){
        main_places_pointer -= 2;
      }
      else{
        main_places_pointer--;
      }
    }
  }
  else{  // IN THE EDITING MODE
    // FUNCTION FOR APPLYING EDITING VALUES HERE MUST BE
    editing_values(where);
  }
}


void update_system(int where){
  if (main_places_pointer == 0){
    if (where < 0 && main_systems_pointer < 6){
      main_systems_pointer++;
    }
    else if (where > 0 && main_systems_pointer > 0){
      main_systems_pointer--;
    };
  }
}



void setup() {
  
  // Инициализация LCD дисплея
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
//  Serial.println(CaseOne[0].system_val);
//  CaseOne[0].set_value(444);
//  Serial.println(CaseOne[0].system_val);
  update_display();
}


void loop() {
  
  // обязательная функция отработки. Должна постоянно опрашиваться
  enc1.tick();
  
  
  if (enc1.isRight()){                  // если был поворот направо
    Serial.println("Right");
    update_pointer(1);
  };         
  
  if (enc1.isLeft()){                   // если был поворот налево
    Serial.println("Left");
    update_pointer(-1);  
  };
  
  if (enc1.isRightH()){
    Serial.println("Right holded");
    update_system(1);
  };                                    // если было удержание + поворот направо

  if (enc1.isLeftH()){
    Serial.println("Left holded");
    update_system(-1);
  };                                    // если было удержание + поворот налева
  
  if (enc1.isPress()){
    Serial.println("Press");            // нажатие на кнопку (+ дебаунс)
  };
  
  if (enc1.isRelease()){
    Serial.println("Release");          // отпускание кнопки (+ дебаунс)
    toggle_editing_mode();
    update_display();
  };
  
  if (enc1.isHolded()){
    Serial.println("Holded");           // если была удержана и энк не поворачивался
  };
  
//  if (enc1.isHold()){
//    Serial.println("Hold");            // возвращает состояние кнопки
//  };


  if (enc1.isTurn()) {                   // если был совершён поворот (индикатор поворота в любую сторону)
    update_display();
   
  }
}
