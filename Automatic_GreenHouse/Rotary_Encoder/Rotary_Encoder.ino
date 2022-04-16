#include <LiquidCrystal_I2C.h>

#define outputC 32 // Key 
#define outputA 33 // S1
#define outputB 25 // S2

// Объявление объекта LCD дисплея
LiquidCrystal_I2C lcd(0x27, 20, 4);

int counter = 0; 
int aState;
int aLastState;
int bPress;


void setup() { 
  // Инициализация LCD дисплея
  lcd.init();
  lcd.backlight();
  
  pinMode (outputA,INPUT);
  pinMode (outputB,INPUT);
  pinMode (outputC,INPUT);

  Serial.begin (9600);
  // Reads the initial state of the outputA
  aLastState = digitalRead(outputA);       

  // Вывод на LCD дисплей
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("Hey!");  

} 


void loop() { 

    bPress = digitalRead(outputC);
     
    aState = digitalRead(outputA); // Reads the "current" state of the outputA
    // If the previous and the current state of the outputA are different, that means a Pulse has occured
    if (bPress == 0){
        Serial.println("Button Pressed!");
        if (aState != aLastState){     
            // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
            if (digitalRead(outputB) != aState) { 
                counter ++;
            } else {
                counter --;
            };
        
            if (counter%2 ==0){
                Serial.print("Position + Press: ");
                Serial.println(counter/2);
            };
        };
         
    }
    else {
        if (aState != aLastState){     
            // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
            if (digitalRead(outputB) != aState) { 
                counter ++;
            } else {
                counter --;
            };

            if (counter%2 ==0){
                Serial.print("Position: ");
                Serial.println(counter/2);
            };      
        };        
    };
    aLastState = aState; // Updates the previous state of the outputA with the current state



//  // Вывод на LCD дисплей
//  lcd.clear();
//  lcd.setCursor(0, 0);  
//  lcd.print("Hey!");  


};
