// Импортирование необходимых библиотек
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <NTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include "RTClib.h"


#define RXD2 16
#define TXD2 17
#define ONBOARD_LED  2
#define countof(a) (sizeof(a) / sizeof(a[0]))

char datestring[20];

int baud = 9600;
int pointer = 0;

// Для хранения значении с датчиков
int temperature;
int humidity;
int light;
int water;
int carbon;
int water_temperature;
int water_level;


int dummy_temperature;
int dummy_humidity;
int dummy_light;
int dummy_water;
int dummy_carbon;
int dummy_water_temperature;
int dummy_water_level;


// Параметры сети WI-FI
const char* ssid = "Le petit dejeuner 2";
const char* password = "DoesGodReallyExist404";

const char* http_username = "micro";
const char* http_password = "micro";

// Объявление объекта LCD дисплея
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Назначение статического IP адреса
IPAddress local_IP(192, 168, 72, 248);
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 255);
IPAddress primaryDNS(8, 8, 8, 8);  
IPAddress secondaryDNS(8, 8, 4, 4);


// Объявление объекта NTP Client для получения времени 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

RTC_DS3231 rtc;

// Создание AsyncWebServer объекта на порте 80
AsyncWebServer server(80);

// Создание Event Source объекта
AsyncEventSource events("/events");

// Интервал обновления показании датчиков и времени на Веб-странице
unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;    // КАЖДЫЕ 10 секунд

bool startOfProgramm = true;

// Переменные для хранения и обработки значении времени для Веб-страницы
String formattedDate;
String dayStamp;
String timeStamp;
String DateTimeStamp;


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
const char* LIGHT_PARAM_INPUT2 = "new-light-value-duration";
const char* LIGHT_PARAM_INPUT3 = "new-light-value";
const char* LIGHT_PARAM_INPUT4 = "new-light-value-repeat";
bool light_repeat = false;

const char* WATER_PARAM_INPUT1 = "new-water-value-time";
const char* WATER_PARAM_INPUT2 = "new-water-value-duration";
const char* WATER_PARAM_INPUT3 = "new-water-value";
const char* WATER_PARAM_INPUT4 = "new-water-value-repeat";
bool water_repeat = false;

const char* TIME_PARAM_INPUT = "new-update-value";

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
String temp_set_value;
String hum_set_value;
String light_set_value;
String water_set_value;
String carbon_set_value;
String water_temp_set_value;
String water_level_set_value;


float temp_set_value_f;
float hum_set_value_f;
float carbon_set_value_f;
float water_temp_set_value_f;
float water_level_set_value_f;


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
      
//      Serial.println();
//      Serial.println("Recieved values: ");
//      Serial.println();
//      Serial.print("temperature: ");
//      Serial.println(temperature);
//      Serial.print("humidity: ");
//      Serial.println(humidity);    
//      Serial.print("light: ");
//      Serial.println(light);
    }
   
    
    if (temperature + humidity + light > 0){
      digitalWrite(ONBOARD_LED,HIGH);
      delay(200);
      digitalWrite(ONBOARD_LED,LOW);
    }
    
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


// ФУНКЦИЯ ДЛЯ ПОДКЛЮЧЕНИЯ WI-FI
void initWiFi() {
  
  // Применение конфигурации для IP адреса
//  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
//    Serial.println("STA Failed to configure");
//  }

    WiFi.onEvent(Wifi_connected,SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(Get_IPAddress, SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(Wifi_disconnected, SYSTEM_EVENT_STA_DISCONNECTED); 
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
//    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
//        Serial.print('.');
        delay(1000);
    }
    
    // Вывод IP адреса на LCD дисплей
    lcd.setCursor(0, 0);  
    lcd.print(WiFi.localIP());

//    Serial.println(WiFi.localIP());
    timeClient.begin();
    timeClient.setTimeOffset(21600);   // 21600 GMT +6 Для Астаны
}


void Wifi_connected(WiFiEvent_t event, WiFiEventInfo_t info){
//  Serial.println("Successfully connected to Access Point");
}

void Get_IPAddress(WiFiEvent_t event, WiFiEventInfo_t info){
//  Serial.println("WIFI is connected!");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
}

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info){
//  Serial.println("Disconnected from WIFI access point");
//  Serial.print("WiFi lost connection. Reason: ");
//  Serial.println(info.disconnected.reason);
//  Serial.println("Reconnecting...");
  WiFi.begin(ssid, password);
}


void printDateTime(const DateTime& dt)
{

    snprintf_P(datestring, 
      countof(datestring),
      PSTR("%02u-%02u-%02u // %02u:%02u:"),
      dt.year(),
      dt.month(),
      dt.day(),
      dt.hour(),
      dt.minute()
      );
//    Serial.print(datestring);
}


// ФУНКЦИЯ ДЛЯ ПОЛУЧЕНИЯ ЗНАЧЕНИИ ДАТЫ И ВРЕМЕНИ
void getDateTime(){

  if (WiFi.status() == WL_CONNECTED){
    
    while(!timeClient.update()) {
      timeClient.forceUpdate();
    }
    formattedDate = timeClient.getFormattedDate();
    
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    // Extract time
    timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-4);    
    DateTimeStamp = dayStamp + " // " + timeStamp;
//    Serial.println("Relying on Wifi Time...");
//    Serial.println(DateTimeStamp);
  }
  else{ 
    DateTime now = rtc.now();
    printDateTime(now);
    DateTimeStamp = datestring;
    
//    Serial.println();
//    Serial.println("Relying on DS1302 Module...");
//    Serial.println(DateTimeStamp);
    
  }
}


// ДЛЯ ЗАМЕНЫ %ШАБЛОНОВ% на Веб-странице
String processor(const String& var){

  getDummySensorReadings();
  getDateTime();
  
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
      return String(to + temp_set_value);
    }
    else{
      return String("");
    };
  } 
  else if(var == "HUM_SET_VALUE"){
    if(is_hum_set){
      return String(to + hum_set_value);
    }
    else{
      return String("");
    };  
  }
  else if(var == "LIGHT_SET_VALUE"){
    if(is_light_set){
      return String(light_set_value);
    }
    else{
      return String("");
    };
  }
  else if(var == "WATER_SET_VALUE"){
    if(is_water_set){
      return String(water_set_value);
    }
    else{
      return String("");
    };
  }
  else if(var == "WATER_TEMP_SET_VALUE"){
    if(is_water_temp_set){
      return String(to + water_temp_set_value);
    }
    else{
      return String("");
    };
  }
  else if(var == "WATER_LEVEL_SET_VALUE"){
    if(is_water_level_set){
      return String(to + water_level_set_value);
    }
    else{
      return String("");
    };
  }
  else if(var == "CARBON_SET_VALUE"){
    if(is_carbon_set){
      return String(to + carbon_set_value);
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
    body {  margin: 0;}
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
          <p><i class="fa fa-shower" style="color:#00add6;"></i> ПОЛИВ</p><p><span class="reading"><span id="water">%WATER%</span> ед.</span></p>
        </div>
        <div class=%IS_CARBON_SET%>
          <p><i class="fa fa-leaf" style="color:#059e8a;"></i> СОДЕРЖАНИЕ CO2</p><p><span class="reading"><span id="carbon">%CARBON%</span><span>%CARBON_SET_VALUE%</span> &percnt;</span></p>
        </div>   
        <div class=%IS_WATER_TEMP_SET%>
          <p><i class="fas fa-water" style="color:#f55442;"></i> ТЕМПЕРАТУРА ВОДЫ В БАКЕ</p><p><span class="reading"><span id="water_temp">%WATER_TEMP%</span><span>%WATER_TEMP_SET_VALUE%</span> &deg;C</span></p>
        </div>   
        <div class=%IS_WATER_LEVEL_SET%>
          <p><i class="fas fa-ruler-vertical" style="color:#00add6;"></i> УРОВЕНЬ ВОДЫ В БАКЕ</p><p><span class="reading"><span id="water_level">%WATER_LEVEL%</span><span>%WATER_LEVEL_SET_VALUE%</span> ед.</span></p>
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
    
    <form id="light-form" class=%LIGHT_BUTTON_STATE% action="/getlight">
      Начало освещ.: <input type="time" id="time" name="new-light-value-time">
      продолж. (ч): <input type="number" name="new-light-value-duration">
      повтор: <input type="checkbox" name="new-light-value-repeat">
      <input id="light-value" class="button-submmit" type="submit" value="Задать">
    </form>
    <form id="light-form-on" class=%LIGHT_BUTTON_STATE% action="/getlighttog">
      <input type="hidden" name="new-light-value" value="toggle-light">
      <input id="light-on" class="button-submmit" type="submit" value=%LIGHT_SUB_BUT_TEXT%>
    </form>
    
    <br><br>

    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ ПОЛИВА -->
    
    <form id="water-form" class=%WATER_BUTTON_STATE% action="/getwater">
      Начало полива: <input type="time" id="time" name="new-water-value-time">
      продолж. (ч): <input type="number" name="new-water-value-duration">
      повтор: <input type="checkbox" name="new-water-value-repeat">
      <input id="water-value" class="button-submmit" type="submit" value="Задать">
    </form>
    <form id="water-form-on" class=%WATER_BUTTON_STATE% action="/getwatertog">
      <input type="hidden" name="new-water-value" value="toggle-water">
      <input id="water-on" class="button-submmit" type="submit" value=%WATER_SUB_BUT_TEXT%>
    </form>
    
    <br><br>

    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ ВЕНТИЛЯЦИИ -->
    
    <form id="fan-form-on" class=%FAN_BUTTON_STATE% action="/getfan">
      Вентилятор:<br>
      <input type="hidden" name="new-fan-value" value="toggle-fan">
      <input id="fan-on" class="button-submmit" type="submit" value=%FAN_SUB_BUT_TEXT%>
    </form>
    
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
 
//window.addEventListener('beforeunload', function (e) {  
//  var xhr = new XMLHttpRequest();
//  xhr.open("GET", "/logout", true);
//  xhr.send();
//});

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
          <p><i class="fa fa-shower" style="color:#00add6;"></i> ПОЛИВ</p><p><span class="reading"><span id="water">%WATER%</span> ед.</span></p>
        </div>
        <div class=%IS_CARBON_SET%>
          <p><i class="fa fa-leaf" style="color:#059e8a;"></i> СОДЕРЖАНИЕ CO2</p><p><span class="reading"><span id="carbon">%CARBON%</span><span>%CARBON_SET_VALUE%</span> &percnt;</span></p>
        </div>   
        <div class=%IS_WATER_TEMP_SET%>
          <p><i class="fas fa-water" style="color:#f55442;"></i> ТЕМПЕРАТУРА ВОДЫ В БАКЕ</p><p><span class="reading"><span id="water_temp">%WATER_TEMP%</span><span>%WATER_TEMP_SET_VALUE%</span> &deg;C</span></p>
        </div>   
        <div class=%IS_WATER_LEVEL_SET%>
          <p><i class="fas fa-ruler-vertical" style="color:#00add6;"></i> УРОВЕНЬ ВОДЫ В БАКЕ</p><p><span class="reading"><span id="water_level">%WATER_LEVEL%</span><span>%WATER_LEVEL_SET_VALUE%</span> ед.</span></p>
        </div>       
    </div>
    
    <br><br>

    <form id="update-form" class="" action="/getupd">
      Интервал обновления(сек) : <input type="number" name="new-update-value" placeholder=%UPDATE_SPAN%>
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
}

</script>
</body>
</html>)rawliteral";


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(baud, SERIAL_8N1);               // Для вывода на монитор                          
  Serial1.begin(baud, SERIAL_8N1, RXD2, TXD2);  // От и К Arduino
  
  // Инициализация LCD дисплея
  lcd.init();
  lcd.backlight();
  
  // Инициализация WIFI
  initWiFi();
  
  pinMode(ONBOARD_LED,OUTPUT); 

  delay(3000);
 
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
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }  


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
    if (request->hasParam(TIME_PARAM_INPUT)) {
        new_update_span = request->getParam(TIME_PARAM_INPUT)->value();
        new_update_span = stringToInt(new_update_span)*1000;
        timerDelay = stringToInt(new_update_span);
    }     
    request->send_P(200, "text/html", settings_html, processor);
  });

  
  // Задание значения для СИСТЕМЫ ОСВЕЩЕНИЯ  -- ЗАДАТЧИК
  server.on("/getlight", HTTP_GET, [](AsyncWebServerRequest *request){
    String light_message_time;
    String light_message_duration;
    String light_message_repeat;
      
    // Контроль ОСВЕЩЕНИЯ
    if (request->hasParam(LIGHT_PARAM_INPUT1) && request->hasParam(LIGHT_PARAM_INPUT2)) {
        light_message_time = request->getParam(LIGHT_PARAM_INPUT1)->value();
        light_message_duration = request->getParam(LIGHT_PARAM_INPUT2)->value();
           
        if (light_message_time != "" && light_message_duration != ""){
          
          if (!light_button_state && !is_light_set){
            Serial1.write('L');  
          }
          
          if (getFeedBack()){
          
            // HAVING SET VALUE FOR LIGHTENING
            if (request->hasParam(LIGHT_PARAM_INPUT4)){
              light_repeat = true;
              light_set_value = "Начало с: " + light_message_time + " прод: " + light_message_duration + " часов" + " (пов.)";
            }
            else{
              light_repeat = false;
              light_set_value = "Начало с: " + light_message_time + " прод: " + light_message_duration + " часов";
            }
                  
            if (!light_button_state){
              light_button_state = true;
            }
            if (!is_light_set){
              is_light_set = true;
            }
          }  
        }   
    }; 
        request->send_P(200, "text/html", index_html, processor);
  });


  // Задание значения для СИСТЕМЫ ОСВЕЩЕНИЯ -- ВКЛ/ВЫКЛ
  server.on("/getlighttog", HTTP_GET, [](AsyncWebServerRequest *request){
    String light_message_toggle;
              
    if (request->hasParam(LIGHT_PARAM_INPUT3)) {
        light_message_toggle = request->getParam(LIGHT_PARAM_INPUT3)->value();
    }

    Serial1.write('L');
      
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
    request->send_P(200, "text/html", index_html, processor);
  });


  // Задание значения для СИСТЕМЫ ПОЛИВА  -- ЗАДАТЧИК
  server.on("/getwater", HTTP_GET, [](AsyncWebServerRequest *request){
    String water_message_time;
    String water_message_duration;
    String water_message_repeat;
      
    // Контроль ПОЛИВА
    if (request->hasParam(WATER_PARAM_INPUT1) && request->hasParam(WATER_PARAM_INPUT2)) {
        water_message_time = request->getParam(WATER_PARAM_INPUT1)->value();
        water_message_duration = request->getParam(WATER_PARAM_INPUT2)->value();
           
        if (water_message_time != "" && water_message_duration != ""){
          
          if (!water_button_state && !is_water_set){
            Serial1.write('Z');  
          }
          
          if (getFeedBack()){
          
            // HAVING SET VALUE FOR WATERING
            if (request->hasParam(WATER_PARAM_INPUT4)){
              water_repeat = true;
              water_set_value = "Начало с: " + water_message_time + " прод: " + water_message_duration + " часов" + " (пов.)";
            }
            else{
              water_repeat = false;
              water_set_value = "Начало с: " + water_message_time + " прод: " + water_message_duration + " часов";
            }
                  
            if (!water_button_state){
              water_button_state = true;
            }
            if (!is_water_set){
              is_water_set = true;
            }
          }  
        }   
    }; 
        request->send_P(200, "text/html", index_html, processor);
  });


  // Задание значения для СИСТЕМЫ ПОЛИВА -- ВКЛ/ВЫКЛ
  server.on("/getwatertog", HTTP_GET, [](AsyncWebServerRequest *request){
    String water_message_toggle;
              
    if (request->hasParam(WATER_PARAM_INPUT3)) {
        water_message_toggle = request->getParam(WATER_PARAM_INPUT3)->value();
    }

    Serial1.write('Z');
      
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
          temp_set_value = temp_message;
          temp_set_value_f = stringToFloat(temp_message);
          
          if (!temp_button_state && !is_temp_set){
            Serial1.write('T');  
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
      }
          
      else if (request->hasParam(TEMP_PARAM_INPUT2)) {
          temp_message = request->getParam(TEMP_PARAM_INPUT2)->value();

          Serial1.write('T');
           
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
          hum_set_value = hum_message;
          hum_set_value_f = stringToFloat(hum_message);
          
          if (!hum_button_state && !is_hum_set){
            Serial1.write('H');                   // !!! NEW VALUE FOR ARDUINO !!!
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
      }
          
      else if (request->hasParam(HUM_PARAM_INPUT2)) {
          hum_message = request->getParam(HUM_PARAM_INPUT2)->value();

          Serial1.write('H');
           
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
          carbon_set_value = carbon_message;
          carbon_set_value_f = stringToFloat(carbon_message);
          
          if (!carbon_button_state && !is_carbon_set){
            Serial1.write('W');                   // !!! NEW VALUE FOR ARDUINO !!!
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
      }
          
      else if (request->hasParam(CARBON_PARAM_INPUT2)) {
          carbon_message = request->getParam(CARBON_PARAM_INPUT2)->value();

          Serial1.write('W');
           
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
          water_temp_set_value = water_temp_message;
          water_temp_set_value_f = stringToFloat(water_temp_message);
          
          if (!water_temp_button_state && !is_water_temp_set){
            Serial1.write('J');                   // !!! NEW VALUE FOR ARDUINO !!!
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
      }
          
      else if (request->hasParam(WATER_TEMP_PARAM_INPUT2)) {
          water_temp_message = request->getParam(WATER_TEMP_PARAM_INPUT2)->value();

          Serial1.write('J');
           
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
          water_level_set_value = water_level_message;
          water_level_set_value_f = stringToFloat(water_level_message);
          
          if (!water_level_button_state && !is_water_level_set){
            Serial1.write('V');                   // !!! NEW VALUE FOR ARDUINO !!!
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
      }
          
      else if (request->hasParam(WATER_LEVEL_PARAM_INPUT2)) {
          water_level_message = request->getParam(WATER_LEVEL_PARAM_INPUT2)->value();

          Serial1.write('V');
           
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
      };
      request->send_P(200, "text/html", index_html, processor);
  });

  

  // Задание значения для СИСТЕМЫ ВОЗДУХООБМЕНА
  server.on("/getfan", HTTP_GET, [](AsyncWebServerRequest *request){
      String fan_message;
      
      // Контроль ВЕНТИЛЯТОРА          
      if (request->hasParam(FAN_PARAM_INPUT)) {
          fan_message = request->getParam(FAN_PARAM_INPUT)->value();
      }

      Serial1.write('F');
      
      if (getFeedBack()){
           
        if (fan_message == "toggle-fan"){     
          if (fan_button_state){
            fan_button_state = false;
          }
          else{
            fan_button_state = true;
          }
        }     
      }
      request->send_P(200, "text/html", index_html, processor);
  });
    
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
//      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.onNotFound(notFound);
  server.begin();
}


// Главный цикл 
void loop() {

  // Сверка значении датчиков и обновление страницы каждые timerDelay секунд (настраиваемые).
  if ((millis() - lastTime) > timerDelay) {
    
    // Получение данных о времени и значении с датчиковы
    getDateTime();
    getSensorsReadings();
    
    dummy_temperature = temperature;
    dummy_humidity = humidity;
    dummy_light = light;

    // Вывод IP адреса и времени на LCD дисплей
    lcd.clear();
    lcd.setCursor(0, 0);  
    lcd.print(WiFi.localIP());
    lcd.setCursor(0, 3);  
    lcd.print(DateTimeStamp);


    // DEBUGGING PART

    Serial.println(temp_set_value_f);
    if (temp_set_value_f != 0){
      if (temp_set_value_f <= temperature){
       
        Serial1.write('b');
        temp_button_state = false;
        is_temp_set = false;
        events.send("Refresh the page","refresher",millis());        
      }
//      else{
//        Serial1.write('d');
//      }
    }
    
    // DEBUGGING PART

    
    // Отправка и Обновление значении на Веб-странице
    events.send("ping",NULL,millis());    
    events.send(String(DateTimeStamp).c_str(),"datetime",millis());
    events.send(String(temperature).c_str(),"temperature",millis());
    events.send(String(humidity).c_str(),"humidity",millis());
    events.send(String(light).c_str(),"light",millis());
    
    lastTime = millis();
  }
}
