#include <SoftwareSerial.h>
SoftwareSerial BTserial(12, 13); // RX | TX 
 
char c = ' ';
 
void setup() {
  Serial.begin(9600);

  pinMode(13, OUTPUT);
  pinMode(12, INPUT);
  pinMode(D2, OUTPUT);
  
  Serial.println("Enter AT commands:");

  
 
  BTserial.begin(9600);  
}
 
void loop() {
    // Keep reading from HM-10 and send to Arduino Serial Monitor
    if (BTserial.available())
    {  
      Serial.println("BTSerial available");
        c = BTserial.read();
        Serial.write(c);
    }
  delay(10);
 
    // Keep reading from Arduino Serial Monitor and send to HM-10
    if (Serial.available())
    {
        c = Serial.read();
        BTserial.write(c);
        //Serial.println("Writing to HM-10");
    }
  delay(10);
}
