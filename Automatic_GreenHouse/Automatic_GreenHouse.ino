#include <Wire.h>  // include the standard Wire library
#include <BH1750.h> // include the BH1750 library
#include <LiquidCrystal_I2C.h>
#include <dht.h>
#include <FastLED.h>


int relay_control_pin = 2;

int red_light_pin= 11;
int green_light_pin = 10;
int blue_light_pin = 9;

// Function for RGB LED
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}

#define LED_PIN     6
#define NUM_LEDS    30

CRGB leds[NUM_LEDS];
 
dht DHT; 
BH1750 GY30; // instantiate a sensor event object
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define dht_apin A0 // Analog Pin sensor is connected to


void setup(){

  pinMode(relay_control_pin, OUTPUT);
  
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);

  Serial.begin(9600); // launch the serial monitor
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

  float lux = GY30.readLightLevel(); // read the light level from the sensor and store it in a variable
 
  DHT.read11(dht_apin);
  delay(2000);//Wait 5 seconds before accessing sensor again.
 
  lcd.setCursor(1,0);
  lcd.print("Hum: ");
  lcd.print(" ");
  lcd.print("Tem: ");
  lcd.print(" ");
  lcd.print("Lux: ");
  
  lcd.setCursor(1,1);
  lcd.print(DHT.humidity);
  lcd.print(" ");
  lcd.print(DHT.temperature);
  lcd.print(" ");
  lcd.print(lux);
  

  for (int i = 0; i <= 29; i++) {
    leds[i] = CRGB ( 255, 0, 255);
    FastLED.show();
  }
      
  

  if (DHT.humidity < 25){
    RGB_color(255, 0, 0); // Red
    }
  else{
    RGB_color(0, 255, 0); // Green
    }

  if (DHT.temperature < 10){
    digitalWrite(relay_control_pin, HIGH);
    }
  else{
    digitalWrite(relay_control_pin, LOW);
    }

}
