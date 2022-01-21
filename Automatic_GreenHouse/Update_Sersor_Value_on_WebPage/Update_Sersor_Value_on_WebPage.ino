/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-sent-events-sse/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <NTPClient.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>

// Replace with your network credentials
const char* ssid = "Le petit dejeuner 2";
const char* password = "";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
String DateTimeStamp;

const char* TEMP_PARAM_INPUT1 = "new-temp-value";
const char* TEMP_PARAM_INPUT2 = "new-temp-value-tog";
const char* LIGHT_PARAM_INPUT1 = "new-light-value-time";
const char* LIGHT_PARAM_INPUT2 = "new-light-value-duration";
const char* LIGHT_PARAM_INPUT3 = "new-light-value";

String temp_button_state = "of";
String light_button_state = "of";

float temperature;
float humidity;
float light;


void getSensorReadings(){
    humidity = 10;
    temperature = 22;
    light = 100;
}

// Initialize WiFi
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
    timeClient.setTimeOffset(21600);   // 21600 GMT +6 for Astana
}

void getDateTime(){

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  
  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-4);    
  DateTimeStamp = dayStamp + " // " + timeStamp;
}

String processor(const String& var){
  getSensorReadings();
  getDateTime();
  
  //Serial.println(var);
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
  else if (var == "TEMP_BUTTON_STATE"){
    if (temp_button_state == "on"){
      return String("buttonON");
    }
    else{
      return String("buttonOFF");
    }
    return String(temp_button_state);
  }
  else if (var == "LIGHT_BUTTON_STATE"){
    if (light_button_state == "on"){
      return String("buttonON");
    }
    else{
      return String("buttonOFF");
    }
    return String(light_button_state);
  };
  return String();
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
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
    <h1>ФИТОФЕРМА КАЗАТУ</h1>
    <p style="display:inline;"><i class="far fa-clock" style="color:#e1e437;"></i> Bремя: </p><p style="display:inline;"><span class="reading"><span id="time">%DATETIME%</span></span></p>
  </div>
 
  <div class="content">
  
    <div class="cards">
        <div class="card">
          <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> ТЕМПЕРАТУРА</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
        </div>
        <div class="card">
          <p><i class="fas fa-tint" style="color:#00add6;"></i> ВЛАЖНОСТЬ</p><p><span class="reading"><span id="hum">%HUMIDITY%</span> &percnt;</span></p>
        </div>
        <div class="card">
          <p><i class="far fa-lightbulb" style="color:#e1e437;"></i> ОСВЯЩЕНИЕ</p><p><span class="reading"><span id="pres">%LIGHT%</span> lux</span></p>
        </div>
    </div>
    
    <br><br>
    
    <form id="temp-form" class=%TEMP_BUTTON_STATE% action="/gettemp">
      Температура на : <input type="text" name="new-temp-value">
      <input id="temp-value" class="button-submmit" type="submit" value="Следить">
    </form>
    <form id="temp-form-on" class=%TEMP_BUTTON_STATE% action="/gettemp">
      <input type="hidden" name="new-temp-value-tog" value="toggle-temp">
      <input id="temp-on" class="button-submmit" type="submit" value="Включить">
    </form>
    
    <br><br>

    <form id="light-form" class=%LIGHT_BUTTON_STATE% action="/getlight">
      Начиная с : <input type="time" name="new-light-value-time">
      продолжительность : <input type="number" name="new-light-value-duration">
      <input id="light-value" class="button-submmit" type="submit" value="Начать">
    </form>
    <form id="light-form-on" class=%LIGHT_BUTTON_STATE% action="/getlight">
      <input type="hidden" name="new-light-value" value="toggle-light">
      <input id="light-on" class="button-submmit" type="submit" value="Включить">
    </form>
    
    <br><br>
    
    <button type="button" class="button-submmit"><a href="/hard-control">Настройки</a></button>

  
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
  console.log("date", e.data);
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

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Handle Web Server
  server.on("/getlight", HTTP_GET, [](AsyncWebServerRequest *request){
    String light_message_time;
    String light_message_duration;
    String light_message_toggle;
      
      // Контроль ОСВЯЩЕНИЯ
      if (request->hasParam(LIGHT_PARAM_INPUT1) && request->hasParam(LIGHT_PARAM_INPUT2)) {
          light_message_time = request->getParam(LIGHT_PARAM_INPUT1)->value();
          light_message_duration = request->getParam(LIGHT_PARAM_INPUT2)->value();
          if (light_message_time != "" && light_message_duration != ""){
            Serial.println("POST REQUEST: " + light_message_time + ": " + light_message_duration);
            // HAVING SET VALUE FOR LIGHTENING
            
            if (light_button_state == "of"){
              light_button_state = "on";
            }
            
          }   
      }
      
      if (light_message_time == "" && light_message_duration == ""){
        if (request->hasParam(LIGHT_PARAM_INPUT3)) {
            light_message_toggle = request->getParam(LIGHT_PARAM_INPUT3)->value();
        }
        
        if(light_message_toggle = "toggle-light"){
          if (light_button_state == "on"){
              light_button_state = "of";
          }
          else{
            light_button_state = "on";
          }
          Serial.println("POST REQUEST: " + light_message_toggle);
        }
      }
         
      request->send_P(200, "text/html", index_html, processor);
  });

  
  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/gettemp", HTTP_GET, [](AsyncWebServerRequest *request){
      String temp_message;
      
      // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
      
      // Контроль ТЕМПЕРАТУРЫ
      if (request->hasParam(TEMP_PARAM_INPUT1)) {
          temp_message = request->getParam(TEMP_PARAM_INPUT1)->value();
          if (temp_message != ""){
            
            // HAVING SET VALUE ON TEMPERATURE
            
            if (temp_button_state == "of"){
              temp_button_state = "on";
            }  
          }
          
      }
          
      else if (request->hasParam(TEMP_PARAM_INPUT2)) {
          temp_message = request->getParam(TEMP_PARAM_INPUT2)->value();
      };
      
      if (temp_message == "toggle-temp"){     
        if (temp_button_state == "on"){
          temp_button_state = "of";
        }
        else{
          temp_button_state = "on";
        }
      }      
   
      Serial.println("POST REQUEST: " + temp_message);
      request->send_P(200, "text/html", index_html, processor);
  });

    
  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {

    //  !!! GETTING VALUES FROM ARDUINO !!!
    getSensorReadings();
    getDateTime();
    
//    Serial.printf("Date: ", dayStamp);
//    Serial.printf("Time: ", timeStamp);
    Serial.printf("Temperature = %.2f ºC \n", temperature);
    Serial.printf("Humidity = %.2f \n", humidity);
    Serial.printf("Light = %.2f hPa \n", light);
    Serial.println();

    // Send Events to the Web Server with the Sensor Readings
    events.send("ping",NULL,millis());
    
    events.send(String(DateTimeStamp).c_str(),"datetime",millis());
    
    events.send(String(temperature).c_str(),"temperature",millis());
    events.send(String(humidity).c_str(),"humidity",millis());
    events.send(String(light).c_str(),"light",millis());
    
    lastTime = millis();
  }
}
