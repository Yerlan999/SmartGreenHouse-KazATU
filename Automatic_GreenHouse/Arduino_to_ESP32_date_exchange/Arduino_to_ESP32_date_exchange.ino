// CODE FOR ARDUINO MEGA

#define RXD2 0
#define TXD2 1

String from_ESP32;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello, ESP32! (P.S from Arduino)");
  delay(1500);
  Serial.println(Serial.readString());
  
}
