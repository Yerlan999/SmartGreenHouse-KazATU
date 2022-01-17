// CODE FOR ESP32

#define RXD2 16
#define TXD2 17

String from_Arduino;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
  // put your main code here, to run repeatedly:
  from_Arduino = Serial1.readString();
  Serial.println(from_Arduino);

  Serial1.println("Hello, Arduino! (P.S from ESP32)");
  delay(600);
}
