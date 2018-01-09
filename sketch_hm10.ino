#include <AltSoftSerial.h>
AltSoftSerial BTSerial;

boolean DEBUG = true;

#define LED_PIN 2
#define rxPin 1
#define txPin 0

// SoftwareSerial mySerial(txPin, rxPin); // RX, TX  
// Connect HM10      Arduino Uno
//     Pin 1/TXD          Pin 7
//     Pin 2/RXD          Pin 8

void setup() {  
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println(__FILE__);
    Serial.println(__DATE__);
    Serial.println(" ");
  }

   //  open software serial connection to the Bluetooth module.
  BTSerial.begin(9600); 
  if (DEBUG)  {   Serial.println("BTSerial started at 9600");
     }
  
  // If the baudrate of the HM-10 module has been updated,
  // you may need to change 9600 by another value
  // Once you have found the correct baudrate,
  // you can update it using AT+BAUDx command 
  // e.g. AT+BAUD0 for 9600 bauds
  
  pinMode(LED_PIN, OUTPUT);
//  pinMode(rxPin, INPUT);
//  pinMode(txPin, OUTPUT);

  }


void loop() {  
  int c;

  
  if (BTSerial.available() > 0) {
    c = BTSerial.read();  
    Serial.println("Got input:");
    if (c != 0)
    {
      // Non-zero input means "turn on LED".
      Serial.println("  on");
      digitalWrite(LED_PIN, HIGH);
    }
    else
    {
      // Input value zero means "turn off LED".
      Serial.println("  off");
      digitalWrite(LED_PIN, LOW);
    }  
  }
}
