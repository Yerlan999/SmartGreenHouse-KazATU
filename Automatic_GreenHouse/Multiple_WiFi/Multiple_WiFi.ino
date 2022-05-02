#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>


int baud = 9600;

WiFiMulti wifiMulti;

// Параметры сети WI-FI
const char* main_ssid = "Le petit dejeuner 2";
const char* main_password = "DoesGodReallyExist404";
const char* other_ssid = "Le petit dejeuner 2";
const char* other_password = "DoesGodReallyExist404";
const char* another_ssid = "Le petit dejeuner 2";
const char* another_password = "DoesGodReallyExist404";


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

    wifiMulti.addAP("Le petit dejeuner 2", "DoesGodReallyExist404");
    wifiMulti.addAP("HUAWEI Y6 2019", "mama1963");
    wifiMulti.addAP("SSID3", "password3");

    
    if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println(WiFi.SSID());
    }
    
//    WiFi.begin(main_ssid, main_password);
////    Serial.print("Connecting to WiFi ..");
//    while (WiFi.status() != WL_CONNECTED) {
////        Serial.print('.');
//        delay(1000);
//    }
    


}


void Wifi_connected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Successfully connected to Access Point");
}

void Get_IPAddress(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WIFI is connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WIFI access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Reconnecting...");

 
  WiFi.begin(main_ssid, main_password);
//  while (WiFi.status() != WL_CONNECTED){
//    if (WiFi.status() == WL_CONNECT_FAILED) {
//      WiFi.begin(other_ssid, other_password);
//      while (WiFi.status() != WL_CONNECTED){
//        if (WiFi.status() == WL_CONNECT_FAILED) {
//          WiFi.begin(another_ssid, another_password);
//          while (WiFi.status() != WL_CONNECTED){ 
//            if (WiFi.status() == WL_CONNECT_FAILED) {
//              break;    
//            }
//            break;
//          }
//        }
//      break;
//      }
//    }    
//  break;
//  }
//  Serial.println(WiFi.SSID());
}


void setup() {
  // put your setup code here, to run once:
  initWiFi();
  Serial.begin(baud);
}

void loop() {
  // put your main code here, to run repeatedly:

}
