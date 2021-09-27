// Example sketch for interfacing with the DS1302 timekeeping chip.
//
// Copyright (c) 2009, Matt Sparks
// All rights reserved.
//
// http://quadpoint.org/projects/arduino-ds1302
#include <Wire.h>  // include the standard Wire library
#include <BH1750.h> // include the BH1750 library
#include <LiquidCrystal_I2C.h>
#include <dht.h>
#include <FastLED.h>
#include <LCD_1602_RUS.h>
#include <stdio.h>
#include <DS1302.h>

namespace {

// Set the appropriate digital I/O pin connections. These are the pin
// assignments for the Arduino as well for as the DS1302 chip. See the DS1302
// datasheet:
//
//   http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
const int kCePin   = 5;  // Chip Enable
const int kIoPin   = 6;  // Input/Output
const int kSclkPin = 7;  // Serial Clock

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Воскресенье";
    case Time::kMonday: return "Понедельник";
    case Time::kTuesday: return "Вторник";
    case Time::kWednesday: return "Среда";
    case Time::kThursday: return "Четверг";
    case Time::kFriday: return "Пятница";
    case Time::kSaturday: return "Суббота";
  }
  return "(unknown day)";
}


Time printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
  return t;
}

}  // namespace


int relay_control_pin = 2;

#define LED_PIN     9
#define NUM_LEDS    30
#define dht_apin A0 // Analog Pin sensor is connected to


dht DHT;
CRGB leds[NUM_LEDS];
BH1750 GY30; // instantiate a sensor event object
LCD_1602_RUS lcd(0x3F, 20, 4);
// LiquidCrystal_I2C lcd(0x3F, 16, 2); // set the LCD address to 0x27/0x3F for a 16/20 chars and 4 line display



void setup() {

  rtc.writeProtect(false);
  rtc.halt(false);

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

  // WRITING ON LCD DISPLAY
  // WRITING ON THE FIRST PAGE OF THE DISPLAY
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Температура воздуха: ");
  lcd.setCursor(0, 1); lcd.print(DHT.temperature);

  // WRITING ON THE SECOND PAGE OF THE DISPLAY
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Влажность воздуха: ");
  lcd.setCursor(0, 1); lcd.print(DHT.humidity);

  // WRITING ON THE THIRD PAGE OF THE DISPLAY
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Уровень освященности: ");
  lcd.setCursor(0, 1); lcd.print(lux);

  // WRITING ON THE FOURTH PAGE OF THE DISPLAY
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Уровень воды: ");
  lcd.setCursor(0, 1); lcd.print(water_level);

  // WRITING ON THE FIFTH PAGE OF THE DISPLAY
  delay(3000);
  lcd.clear();
  
  Time test = printTime();
  int year = test.yr; int month = test.mon; int date = test.date;
  int hour = test.hr; int minute = test.min; int second = test.sec;
  const String week_day = dayAsString(test.day);
  
  lcd.setCursor(0, 0);
  lcd.print(date); lcd.print("/");
  lcd.print(month); lcd.print("/");
  lcd.print(year);
  
  lcd.setCursor(0, 1);
  lcd.print(hour); lcd.print(":");
  lcd.print(minute); lcd.print(":");
  lcd.print(second);





  
  
  // OVERHEAD GROWING LED CONTROL
  for (int i = 0; i <= 29; i++) {
    leds[i] = CRGB ( 250, 0, 255);
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
