String to_ESP32 = "To ESP32 from ARDUINO";
String from_ESP32;

void setup() {
  Serial.begin(9600);    // Displaying on monitor
  Serial1.begin(9600, SERIAL_8N1);   // Communication with ESP32
}

void loop() {
  if (Serial1.available()>0){
    from_ESP32 = Serial1.readString();
    if (from_ESP32 == "S"){
      Serial1.print("Here are your sensors");
    }
    if (from_ESP32.startsWith("A")){
      Serial1.print("Executing actuators");
    }
  }
}
