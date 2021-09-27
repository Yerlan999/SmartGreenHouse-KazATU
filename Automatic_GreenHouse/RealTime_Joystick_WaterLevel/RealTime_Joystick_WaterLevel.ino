#include <Wire.h>  // include the standard Wire library
#include <BH1750.h> // include the BH1750 library
#include <LiquidCrystal_I2C.h>
#include <dht.h>
#include <FastLED.h>
#include <LCD_1602_RUS.h>


#define LED_PIN     9
#define NUM_LEDS    30
#define dht_apin A0 // Analog Pin sensor is connected to
#define outputC 3
#define outputA 4
#define outputB 5


CRGB leds[NUM_LEDS];
dht DHT;
BH1750 GY30; // instantiate a sensor event object
LCD_1602_RUS lcd(0x3F, 20, 4);
// LiquidCrystal_I2C lcd(0x3F, 16, 2); // set the LCD address to 0x27/0x3F for a 16/20 chars and 4 line display

int counter = 0; 
int aState;
int aLastState;
int bPress;
int last_counter = 0;  
int relay_control_pin = 2;
int display_pages[4] = {0,1,2,3};
int current_page;
float value2show;
char text2show;


int change_page(int count) {
    int page_num;
    
    if (count > 3){
        page_num = 3; 
    }
    else if (count < 0){
        page_num = 0;
    }
    else {
        page_num = count;
      };
    return page_num;
}

void setup() { 
  
    pinMode (outputA,INPUT);
    pinMode (outputB,INPUT);
    pinMode (outputC,INPUT);
    pinMode (relay_control_pin, OUTPUT);

    Serial.begin (9600);
    // Reads the initial state of the outputA
    aLastState = digitalRead(outputA);   
    
    Wire.begin(); // Initialize the I2C bus for use by the BH1750 library
    GY30.begin(); // Initialize the sensor object
  
    lcd.init();                      // initialize the lcd
    lcd.init();
    // Print a message to the LCD.
    lcd.backlight();
    delay(500);//Delay to let system boot
    delay(1000);//Wait before accessing Sensor
  
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
} 


void loop() { 
    



    













  
    bPress = digitalRead(outputC);
     
    aState = digitalRead(outputA); // Reads the "current" state of the outputA
    // If the previous and the current state of the outputA are different, that means a Pulse has occured
    if (bPress == 0){
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
                
                current_page = change_page(counter/2);
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
                
                current_page = change_page(counter/2);
            };      
        };        
    };
    aLastState = aState; // Updates the previous state of the outputA with the current state




    Serial.println(current_page);
//    // FIRST ROW
//    lcd.setCursor(0, 0);
//    lcd.print(text2show);
//    // SECOND ROW
//    lcd.setCursor(0, 1);
//    lcd.print(value2show);








//    // READING FROM SENSORS
//    float lux = GY30.readLightLevel(); // read the light level from the sensor and store it in a variable
//    int water_level = analogRead(A1);
//    DHT.read11(dht_apin);
//    delay(2000);//Wait 5 seconds before accessing sensor again.
//
//
//    // OVERHEAD GROWING LED LIGHT CONTROL
//    for (int i = 0; i <= 29; i++) {
//      leds[i] = CRGB ( 250, 0, 255);
//      FastLED.show();
//    }
//
//
//    // PUMP CONTROL BY WATER LEVEL
//    if (water_level > 200){
//      digitalWrite(relay_control_pin, LOW);
//      }
//    else {
//      digitalWrite(relay_control_pin, HIGH);
//      };


















};
