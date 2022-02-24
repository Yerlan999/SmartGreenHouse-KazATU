// Импортирование необходимых библиотек
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <NTPClient.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>

// Параметры сети WI-FI
const char* ssid = "";
const char* password = "";

// Объявление объекта NTP Client для получения времени 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Создание AsyncWebServer объекта на порте 80
AsyncWebServer server(80);

// Создание Event Source объекта
AsyncEventSource events("/events");

// Интервал обновления показании датчиков и времени на Веб-странице
unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;

// Переменные для хранения и обработки значении времени для Веб-страницы
String formattedDate;
String dayStamp;
String timeStamp;
String DateTimeStamp;

const char* FAN_PARAM_INPUT = "new-fan-value";
const char* TEMP_PARAM_INPUT1 = "new-temp-value";
const char* TEMP_PARAM_INPUT2 = "new-temp-value-tog";
const char* LIGHT_PARAM_INPUT1 = "new-light-value-time";
const char* LIGHT_PARAM_INPUT2 = "new-light-value-duration";
const char* LIGHT_PARAM_INPUT3 = "new-light-value";

// Состояния кнопок контроля
bool temp_button_state = false;
bool light_button_state = false;
bool fan_button_state = false;

// Состояние карточек отображения показании
bool is_temp_set = false;
bool is_hum_set = false;
bool is_light_set = false;

// Заданные значения для системы
String to = " >>> ";
String temp_set_value;
String hum_set_value;
String light_set_value;

float temp_set_value_f;

// Для хранения значении с датчиков
float temperature;
float humidity;
float light;

// ФУНКЦИЯ ДЛЯ СЧИТЫВАНИЯ С ДАТЧИКОВ ПОКАЗАНИИ
void getSensorReadings(){
    humidity = 10;
    temperature = 22;
    light = 100;
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
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
    timeClient.begin();
    timeClient.setTimeOffset(21600);   // 21600 GMT +6 Для Астаны
}

// ФУНКЦИЯ ДЛЯ ПОЛУЧЕНИЯ ЗНАЧЕНИИ ДАТЫ И ВРЕМЕНИ
void getDateTime(){

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-4);    
  DateTimeStamp = dayStamp + " // " + timeStamp;
}

// ДЛЯ ЗАМЕНЫ %ШАБЛОНОВ% на Веб-странице
String processor(const String& var){
  getSensorReadings();
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
  };
  return String();
}

// !!! HTML СТРАНИЦА !!!
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
          <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> ТЕМПЕРАТУРА</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span><span>%TEMP_SET_VALUE%</span> &deg;C</span></p>
        </div>
        <div class=%IS_HUM_SET%>
          <p><i class="fas fa-tint" style="color:#00add6;"></i> ВЛАЖНОСТЬ</p><p><span class="reading"><span id="hum">%HUMIDITY%</span><span>%HUM_SET_VALUE%</span> &percnt;</span></p>
        </div>
        <div class=%IS_LIGHT_SET%>
          <p>%LIGHT_SET_VALUE%</p>
          <p><i class="far fa-lightbulb" style="color:#e1e437;"></i> ОСВЯЩЕНИЕ</p><p><span class="reading"><span id="pres">%LIGHT%</span> lux</span></p>
        </div>
    </div>
    
    <br><br>

    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ ОТОПЛЕНИЯ -->
    
    <form id="temp-form" class=%TEMP_BUTTON_STATE% action="/gettemp">
      Температура на : <input type="number" name="new-temp-value">
      <input id="temp-value" class="button-submmit" type="submit" value="Задать">
    </form>
    <form id="temp-form-on" class=%TEMP_BUTTON_STATE% action="/gettemp">
      <input type="hidden" name="new-temp-value-tog" value="toggle-temp">
      <input id="temp-on" class="button-submmit" type="submit" value="Включить">
    </form>
    
    <br><br>
    
    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ ОСВЯЩЕНИЯ -->
    
    <form id="light-form" class=%LIGHT_BUTTON_STATE% action="/getlight">
      Начиная с : <input type="time" id="time" name="new-light-value-time">
      продолжительность (ч.): <input type="number" name="new-light-value-duration">
      <input id="light-value" class="button-submmit" type="submit" value="Задать">
    </form>
    <form id="light-form-on" class=%LIGHT_BUTTON_STATE% action="/getlighttog">
      <input type="hidden" name="new-light-value" value="toggle-light">
      <input id="light-on" class="button-submmit" type="submit" value="Включить">
    </form>
    
    <br><br>

    <!-- КНОПКА ДЛЯ КОНТРОЛЯ СИСТЕМЫ ВЕНТИЛЯЦИИ -->
    
    <form id="fan-form-on" class=%FAN_BUTTON_STATE% action="/getfan">
      Вентилятор:<br>
      <input type="hidden" name="new-fan-value" value="toggle-fan">
      <input id="fan-on" class="button-submmit" type="submit" value="Включить">
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
  
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/", true);
  xhr.send();
  
 }, false);
}

</script>
</body>
</html>)rawliteral";


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}


void setup() {
  Serial.begin(115200);
  initWiFi();  

  // Главная страница
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  
  // Страница настроек
  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  
  // Задание значения для СИСТЕМЫ ОСВЯЩЕНИЯ  -- ЗАДАТЧИК
  server.on("/getlight", HTTP_GET, [](AsyncWebServerRequest *request){
    String light_message_time;
    String light_message_duration;
      
    // Контроль ОСВЯЩЕНИЯ
    if (request->hasParam(LIGHT_PARAM_INPUT1) && request->hasParam(LIGHT_PARAM_INPUT2)) {
        light_message_time = request->getParam(LIGHT_PARAM_INPUT1)->value();
        light_message_duration = request->getParam(LIGHT_PARAM_INPUT2)->value();
        
        if (light_message_time != "" && light_message_duration != ""){
          Serial.println("POST REQUEST: " + light_message_time + ": " + light_message_duration);
          
          // HAVING SET VALUE FOR LIGHTENING
          light_set_value = "Начало с: " + light_message_time + " прод: " + light_message_duration + " часов";
          
          if (!light_button_state){
            light_button_state = true;
          }
          if (!is_light_set){
            is_light_set = true;
          }
          
        }   
    };
          
    request->send_P(200, "text/html", index_html, processor);
  });

  // Задание значения для СИСТЕМЫ ОСВЯЩЕНИЯ -- ВКЛ/ВЫКЛ
  server.on("/getlighttog", HTTP_GET, [](AsyncWebServerRequest *request){
    String light_message_toggle;
              
    if (request->hasParam(LIGHT_PARAM_INPUT3)) {
        light_message_toggle = request->getParam(LIGHT_PARAM_INPUT3)->value();
    }
    
    if(light_message_toggle = "toggle-light"){
      if (light_button_state){
          light_button_state = false;
          is_light_set = false;
      }
      else{
        light_button_state = true;
      }
      Serial.println("POST REQUEST: " + light_message_toggle);
    }
         
    request->send_P(200, "text/html", index_html, processor);
  });


  // Задание значения для СИСТЕМЫ ОТОПЛЕНИЯ
  server.on("/gettemp", HTTP_GET, [](AsyncWebServerRequest *request){
      String temp_message;
      
      // Контроль ТЕМПЕРАТУРЫ
      if (request->hasParam(TEMP_PARAM_INPUT1)) {
          temp_message = request->getParam(TEMP_PARAM_INPUT1)->value();
          if (temp_message != ""){
            
            // HAVING SET VALUE ON TEMPERATURE
            temp_set_value = temp_message;
            temp_set_value_f = stringToFloat(temp_message);
            
            Serial.println(temp_set_value_f > 10);
            Serial.println(temp_set_value_f);
            
            if (!temp_button_state){
              temp_button_state = true;
            }
            if (!is_temp_set){
              is_temp_set = true;  
            }  
          }
          
      }
          
      else if (request->hasParam(TEMP_PARAM_INPUT2)) {
          temp_message = request->getParam(TEMP_PARAM_INPUT2)->value();
      };
      
      if (temp_message == "toggle-temp"){     
        if (temp_button_state){
          temp_button_state = false;
          is_temp_set = false;
        }
        else{
          temp_button_state = true;
        }
      }      
   
      Serial.println("POST REQUEST: " + temp_message);
      request->send_P(200, "text/html", index_html, processor);
  });

  // Задание значения для СИСТЕМЫ ВОЗДУХООБМЕНА
  server.on("/getfan", HTTP_GET, [](AsyncWebServerRequest *request){
      String fan_message;
      
      // Контроль ВЕНТИЛЯТОРА          
      if (request->hasParam(FAN_PARAM_INPUT)) {
          fan_message = request->getParam(FAN_PARAM_INPUT)->value();
      }
      
      if (fan_message == "toggle-fan"){     
        if (fan_button_state){
          fan_button_state = false;
        }
        else{
          fan_button_state = true;
        }
      }      
   
      Serial.println("POST REQUEST: " + fan_message);
      request->send_P(200, "text/html", index_html, processor);
  });
    
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.onNotFound(notFound);
  server.begin();
}

// Главный цикл 
void loop() {
  if ((millis() - lastTime) > timerDelay) {

    //  !!! Получение даннах от Arduino !!!
    getSensorReadings();
    getDateTime();

    // Отправка и Обновление значении на Веб-странице
    events.send("ping",NULL,millis());
    
    events.send(String(DateTimeStamp).c_str(),"datetime",millis());
    
    events.send(String(temperature).c_str(),"temperature",millis());
    events.send(String(humidity).c_str(),"humidity",millis());
    events.send(String(light).c_str(),"light",millis());
    
    lastTime = millis();
  }
}
