#define RXD2 16
#define TXD2 17

int pointer = 1;
int values[3];
byte InValues[6];
int charCount;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
  
  while (Serial.available()>0){
    
    if (Serial.read() == 48){
      Serial1.write('A'); 
    }
  }
   
  while (Serial1.available()>0){
    charCount = Serial1.readBytes((uint8_t*)InValues, sizeof(InValues));
//    if (inByte == '\n'){
//      Serial.println("New Line Symbol");
//    }
    Serial.print("Char count: ");
    Serial.println(charCount);
     
  }
}
