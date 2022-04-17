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

char hover_cursor = '>';
char select_cursor = '=';
char switch_cursos = '#';

bool editing_mode = false;

void toggle_editing_mode(){
  if (main_places_pointer != 0){
    if (editing_mode){
      editing_mode = false;
    }
    else{
      editing_mode = true;
    }
  }
}


void update_display(){
  lcd.clear();
  lcd.setCursor(options[main_places_pointer].col, options[main_places_pointer].row);
  lcd.print(update_cursor_type());

  lcd.setCursor(4, 0);
  lcd.print(system_titles[main_systems_pointer].system_name);
}


char update_cursor_type(){
  if (editing_mode){
    return select_cursor;
  }
  else{
    return hover_cursor;
  }

}


void update_pointer(int where){
  if (!editing_mode){
    if (where < 0 && main_places_pointer < 4){
      main_places_pointer++;
    }
    else if (where > 0 && main_places_pointer > 0){
      main_places_pointer--;
    };
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


void update_value(int where){
  
}


void diplay_correspond_menu(int what){
  
}



void setup() {
  
  // Инициализация LCD дисплея
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);

  // Вывод на LCD дисплей
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print(update_cursor_type());

  lcd.setCursor(4, 0);
  lcd.print(system_titles[0].system_name);
}


void loop() {

//  Serial.println(options[3].place);
//  Serial.println(options[3].row);
//  Serial.println(options[3].col);

  
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
