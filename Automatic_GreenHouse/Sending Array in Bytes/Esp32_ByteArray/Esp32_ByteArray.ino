//    Serial.println(Serial1.readBytes(buf, 6));

#define RXD2 16
#define TXD2 17

int pointer = 1;
int values[3];

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
  
  while (Serial.available()>0){
    
    if (Serial.read() == 48){
      Serial1.write('A'); 
    }
  }
   
  while (Serial1.available()>0){
    byte inByte = Serial1.read();
    if (inByte == '\n'){
      Serial.println("New Line Symbol");
    }
    Serial.println(inByte);  
  }
}
