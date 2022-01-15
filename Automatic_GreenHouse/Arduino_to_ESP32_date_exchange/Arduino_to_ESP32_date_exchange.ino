// CODE FOR ARDUINO MEGA

#define RXD2 0
#define TXD2 1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//  Serial1.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello, ESP32!");
  delay(1500);
//  Serial.println(Serial1.readString());
  
}
