#include <Wire.h>  // include the standard Wire library
#include <BH1750.h> // include the BH1750 library
#include <LiquidCrystal_I2C.h>
#include <dht.h>
#include <FastLED.h>
#include <LCD_1602_RUS.h>


int relay_control_pin = 2;

#define LED_PIN     9
#define NUM_LEDS    30

CRGB leds[NUM_LEDS];

dht DHT;
BH1750 GY30; // instantiate a sensor event object
LCD_1602_RUS lcd(0x3F, 16, 2);
// LiquidCrystal_I2C lcd(0x3F, 16, 2); // set the LCD address to 0x27/0x3F for a 16/20 chars and 4 line display

#define dht_apin A0 // Analog Pin sensor is connected to


void setup() {

  pinMode(relay_control_pin, OUTPUT);
  
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

  
  // READING FROM SENSORS
  float lux = GY30.readLightLevel(); // read the light level from the sensor and store it in a variable
  int water_level = analogRead(A1);
  DHT.read11(dht_apin);
  delay(2000);//Wait 5 seconds before accessing sensor again.


  // WRITING ON LCD DISPLAY
  // FIRST ROW
  lcd.setCursor(0, 0);
  lcd.print("Вла: "); lcd.print(" "); lcd.print("Тем: "); lcd.print(" "); lcd.print("Осв: ");
  // SECOND ROW
  lcd.setCursor(0, 1);
  lcd.print(DHT.humidity); lcd.print(" "); lcd.print(DHT.temperature); lcd.print(" "); lcd.print(lux);


  // OVERHEAD GROWING LED CONTROL
  for (int i = 0; i <= 29; i++) {
    leds[i] = CRGB ( 0, 0, 15);
    FastLED.show();
  }

  // PUMP CONTROL BY WATER LEVEL
  if (water_level > 200){
    digitalWrite(relay_control_pin, LOW);
    }
  else {
    digitalWrite(relay_control_pin, HIGH);
    };
   
  
}
