#define RXD2 16
#define TXD2 17

String to_Arduino;
String from_Arduino;

unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;

void setup() {
  Serial.begin(9600);                           // Displaying on monitor                          
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);  // Communication with ARDUINO
}


void requestSensors(){
  Serial1.print("S");
  delay(3000);
  if (Serial1.available()>0){
    from_Arduino = Serial1.readString();
    Serial.println(from_Arduino); 
  }  
}


void requestActuators(){
  Serial1.print("A<some actuators values>");
  delay(3000);
  if (Serial1.available()>0){
    from_Arduino = Serial1.readString();
    Serial.println(from_Arduino); 
  }     
}


void loop() {
  if ((millis() - lastTime) > timerDelay) {
    requestSensors();
    lastTime = millis();
  }
}
