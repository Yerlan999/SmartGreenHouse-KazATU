// CODE FOR ESP32

#define RXD2 16
#define TXD2 17

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(Serial1.readString());
  delay(1500);
  Serial.println("Thanks!");
  
}
