#include "FS.h"
#include "SD.h"
#include <SPI.h>


int test_var1;
float test_var2;
bool test_var3;


typedef struct { 
  uint8_t system_num;
  String system_name;
} L1systems;
const L1systems L1system_titles[] {
    {0, "Temperature"},
    {1, "CO2 Content"},
    {2, "Humidity"},
    {3, "Water Temp."},
    {4, "Water Level"},
    {5, "Watering"},
    {6, "Lightening"},
};


// Define CS pin for the SD card module
#define SD_CS 5

void setup() {
  Serial.begin(9600);

  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed!");Serial.println(" ");
  }
  else {
    Serial.println("Card reader initialized successfully!");Serial.println(" ");
  }
  
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached!");Serial.println(" ");
    return;
  }
  else{
    Serial.println("Card has been found. Ok!");Serial.println(" ");
  }


  // Delete if needed
//  deleteFile(SD, "/sersors_logger.txt");
//  deleteFile(SD, "/actuators_logger.txt");


  if(!SD.exists("/sersors_logger.txt")) {
    Serial.println("'/sersors_logger.txt' file doens't exist");
    Serial.println("Creating file...'/sersors_logger.txt'");
    writeFile(SD, "/sersors_logger.txt", "Temperatre,Humidity,Lighting,Watering,WaterTemperature,WaterLevel,CO2");Serial.println(" ");
  }
  else {
    Serial.println("'/sersors_logger.txt' file already exists!");Serial.println(" ");
  }

  if(!SD.exists("/actuators_logger.txt")) {
    Serial.println("'/actuators_logger.txt' file doens't exist");
    Serial.println("Creating file...'/actuators_logger.txt'");
    writeFile(SD, "/actuators_logger.txt", "PumpState,AirHeaterState,AirHumiditerState,WaterHeaterState,FanState,OutletFanState,PhytolampState,WaterTankFillerState");Serial.println(" ");
  }
  else {
    Serial.println("'/actuators_logger.txt' file already exists!");Serial.println(" ");
  }


//  makeSnapShot(5, "c,23,59,1,0");
//  readSnapShot(5);

}


void loop() {
//  clearFile(SD, "/data.txt");
//  readFile(SD, "/data.txt");
//    delay(30000);   //Wait for 30 seconds before writing the next data 
}



String filePathCreator(int which_system){
  /*
  which_system: 
    0 = "Temperature"
    1 = "CO2 Content"
    2 = "Humidity"
    3 = "Water Temp."
    4 = "Water Level"
    5 = "Watering"
    6 = "Lightening"
  */
  return "/" + L1system_titles[which_system].system_name + ".txt";
}


void makeSnapShot(int which_system, String values){  
  writeFile(SD, filePathCreator(which_system), values);
}

void readSnapShot(int which_system){
  readFile(SD, filePathCreator(which_system));
}



void writeFile(fs::FS &fs, String path, String message) {
  Serial.println("Writing to file: " + path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file " + path + " for writing!");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message to file " + path + " is written!");
  } else {
    Serial.println("Write to file " + path + " failed!");
  }
  file.close();    
}


void appendFile(fs::FS &fs, String path, String message) {
  Serial.println("Appending to file: " + path);
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file " + path + " for appending!");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended to file " + path + "!");
  } else {
    Serial.println("Append failed to file " + path + "!");
  }
  file.close();
}


void readFile(fs::FS &fs, String path) {
  Serial.println("Reading from file: " + path);
  File file = fs.open(path, FILE_READ);
  if(!file) {
    Serial.println("Failed to open file " + path + " for reading!");
    return;
  }
  while (file.available()) {
    Serial.println(file.read());
  }
  file.close();
}


void cleanFile(fs::FS &fs, String path) {
  
  Serial.println("Cleaning file: " + path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file " + path + " for cleaning!");
    return;
  }
  if(file.print(' ')) {
    Serial.println("File " + path + " cleaned!");
  } else {
    Serial.println("Cleaning failed for file " + path + "!");
  }
  file.close();
}


void deleteFile(fs::FS &fs, String path){
  if (fs.remove(path)){
    Serial.println("Successfully deleted file " + path + "!");
  }
  else{
    Serial.println("Failed with deleting file" + path + "!");
  }
}
