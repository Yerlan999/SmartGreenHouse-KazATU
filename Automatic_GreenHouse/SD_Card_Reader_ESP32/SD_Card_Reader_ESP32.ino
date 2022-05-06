#include "FS.h"
#include "SD.h"
#include <SPI.h>

// Define CS pin for the SD card module
#define SD_CS 5

void setup() {
  Serial.begin(9600);

  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
  }
  else {
    Serial.println("Card reader initialized successfully!");
  }
  
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  else{
    Serial.println("Card has been found. Ok!");
  }
  
  if(!SD.exists("/data.txt")) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "Header");
  }
  else {
    Serial.println("File already exists");  
  }
}


void loop() {
  clearFile(SD, "/data.txt");
  readFile(SD, "/data.txt");
//    logSDCard();
    delay(30000);   //Wait for 30 seconds before writing the next data 
}

// Write the sensor readings on the SD card
void logSDCard() {

  appendFile(SD, "/data.txt", "Testing SD Module");
}
// Write to the SD card (DON'T MODIFY THIS FUNCTION)


void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written!");
  } else {
    Serial.println("Write failed!");
  }
  file.close();
}


// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending!");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended!");
  } else {
    Serial.println("Append failed!");
  }
  file.close();
}


// Read data from the SD card (DON'T MODIFY THIS FUNCTION)
void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading from file: %s\n", path);
  File file = fs.open(path, FILE_READ);
  if(!file) {
    Serial.println("Failed to open file for reading!");
    return;
  }
  while (file.available()) {
    Serial.println(file.read());
  }
  file.close();
}


void clearFile(fs::FS &fs, const char * path) {
  Serial.printf("Clearing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for clearing");
    return;
  }
  if(file.print(' ')) {
    Serial.println("File cleared!");
  } else {
    Serial.println("Clearing failed");
  }
  file.close();
}
