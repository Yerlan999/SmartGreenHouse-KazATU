#include "FS.h"
#include "SD.h"
#include <SPI.h>


int test_var1;
float test_var2;
bool test_var3;



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


void init_sd_card(){
  
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
}



void prepare_main_files(){

  // Delete if needed
//  deleteFile(SD, "/sersors_logger.txt");
//  deleteFile(SD, "/actuators_logger.txt");


  if(!SD.exists("/sersors_logger.txt")) {
    Serial.println("'/sersors_logger.txt' file doens't exist");
    Serial.println("Creating file...'/sersors_logger.txt'");
    writeFile(SD, "/sersors_logger.txt", "Date,Time,Temperatre,Humidity,Lighting,Watering,WaterTemperature,WaterLevel,CO2");Serial.println(" ");
  }
  else {
    Serial.println("'/sersors_logger.txt' file already exists!");Serial.println(" ");
  }

  if(!SD.exists("/actuators_logger.txt")) {
    Serial.println("'/actuators_logger.txt' file doens't exist");
    Serial.println("Creating file...'/actuators_logger.txt'");
    writeFile(SD, "/actuators_logger.txt", "Date,Time,PumpState,AirHeaterState,AirHumiditerState,WaterHeaterState,FanState,OutletFanState,PhytolampState,WaterTankFillerState");Serial.println(" ");
  }
  else {
    Serial.println("'/actuators_logger.txt' file already exists!");Serial.println(" ");
  }  
}


void setup() {
  Serial.begin(9600);

  init_sd_card();
  prepare_main_files();


  makeSnapShot(5, "c,23,59,1,");
  readSnapShot(5);

}


void loop() {
//  clearFile(SD, "/data.txt");
//  readFile(SD, "/data.txt");
//    delay(30000);   //Wait for 30 seconds before writing the next data 
}



String filePathCreator(int which_system){
  /* 
   *  !!! RELIES ON TITLES INDEXING PRINCIPLE !!!
   *  
  which_system: 
    0 = "Temperature"
    1 = "CO2 Content"
    2 = "Humidity"
    3 = "Water Temp."
    4 = "Water Level"
    5 = "Watering"
    6 = "Lightening"
  */
  String file_name;
  
  switch (which_system){
    case 0:
     file_name = "";
    break;
    case 1:
     file_name = "";
    break;
    case 2:
     file_name = "";
    break;
    case 3:
     file_name = "";
    break;
    case 4:
     file_name = "";
    break;
    case 5:
     file_name = "";
    break;
    case 6:
     file_name = "";
    break;                    
  }
  return "/" + L1system_titles[which_system].system_name + ".txt";
}


void makeSnapShot(int which_system, String values){  
  writeFile(SD, filePathCreator(which_system), values);
}

void readSnapShot(int which_system){
  readFile(SD, filePathCreator(which_system), which_system);
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


void readFile(fs::FS &fs, String path, int which_system) {
  Serial.println("Reading from file: " + path);
  File file = fs.open(path, FILE_READ);
  if(!file) {
    Serial.println("Failed to open file " + path + " for reading!");
    return;
  }
  
  if (file.available()){
    int set_type = file.read();
    int trash_comma = file.read();
      
    String list = file.readStringUntil('\r');
    Serial.println(set_type);
    Serial.println(list);
    
    String number;
    int seq = 0;
      
    for (int i=0; i<list.length(); i++){
      
      String character;
      
      if (list[i]==','){        
        Serial.println(seq);
        Serial.println(stringToInt(number));
        
        // Reseting system values
        system_setterBySD();        
        
        number = "";
        seq++;
      }
      else{
        character = list[i];
        number+= character;  
      }
      
    }
    
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



void system_setterBySD(){
  // CASE TWO
  if (set_type == 99){ // clock [c,hour,minute,dur1,rep,is_clock_set]
    CaseTwo[which_system-5].set_value(seq+1, number);
    if (seq==4){CaseTwo[which_system].set_value(8, number);}
  }
  else if (set_type == 105){ // interval [i,dur2,pause,is_inter_set]
    CaseTwo[which_system-5].set_value(seq+5, number);
    if (seq==2){CaseTwo[which_system].set_value(9, number);}
  }
  else if (set_type == 107){ // state [k,state,state_set]
    CaseTwo[which_system-5].set_value(seq+7, number);
    if (seq==1){CaseTwo[which_system].set_value(10, number);}
  }

  // CASE ONE
  else if (set_type == 111){ // set
    CaseOne[which_system].set_value(seq+1, number);
    if (seq==1){CaseTwo[which_system].set_value(3, number);}
  }
  else if (set_type == 115){ // state
    CaseOne[which_system].set_value(seq+2, number);
    if (seq==1){CaseTwo[which_system].set_value(4, number);}
  }
}



/* OLD READ FILE FUNCTION
 * void readFile(fs::FS &fs, String path) {
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
}*/
