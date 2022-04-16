#include <LiquidCrystal_I2C.h>

/* 
        CLK == S1
        DT == S2
        SW == Key 
*/

#define CLK 33 // S1
#define DT 25  // S2
#define SW 32  // Key
#include "GyverEncoder.h"
Encoder enc1(CLK, DT, SW);

// Объявление объекта LCD дисплея
LiquidCrystal_I2C lcd(0x27, 20, 4);


void setup() {
  // Инициализация LCD дисплея
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);

  // Вывод на LCD дисплей
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("Hey!");  

}


void loop() {
  // обязательная функция отработки. Должна постоянно опрашиваться
  enc1.tick();
  if (enc1.isTurn()) {     // если был совершён поворот (индикатор поворота в любую сторону)
  // ваш код
  }
  if (enc1.isRight()) Serial.println("Right");         // если был поворот
  if (enc1.isLeft()) Serial.println("Left");
  if (enc1.isRightH()) Serial.println("Right holded"); // если было удержание + поворот
  if (enc1.isLeftH()) Serial.println("Left holded");
  if (enc1.isPress()) Serial.println("Press");         // нажатие на кнопку (+ дебаунс)
  if (enc1.isRelease()) Serial.println("Release");     // отпускание кнопки (+ дебаунс)
  if (enc1.isHolded()) Serial.println("Holded");       // если была удержана и энк не поворачивался
  //if (enc1.isHold()) Serial.println("Hold");         // возвращает состояние кнопки
  delay(100);

//  // Вывод на LCD дисплей
//  lcd.clear();
//  lcd.setCursor(0, 0);  
//  lcd.print("Hey!");  

}
