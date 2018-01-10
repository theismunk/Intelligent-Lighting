/*
 * Main script
 */

#include <ESP8266WiFi.h>  // the ESP8266WiFi.h  lib
const char* SSID = "DTU-Networks";
const char* PASS = "Fotonik343";
char server[] = "mail.smtp2go.com";
ADC_MODE(ADC_VCC);

WiFiClient client;

int ledPin = D2;                  // pin for LED
int inputPin = D13;               // pin for PIR sensor
int state = LOW;                  // initially no motion detected 
int var = 0;                      // variable for reading the pin status
unsigned long timevar = 300000;   // variable for counting time since last email
unsigned long timer = 0;          // variable to delay turning off the light if no movement is detected
int emailinterval = 150000;       // time interval between e-mails sent
int timerinterval = 5000;         // time interval from no motion detected to light is turned off

byte sendEmail(); // function prototype
byte eRcv();      // function prototype


//BLE HM-10 INITIALIZATION
int c;            // variable to store security setting

#include <SoftwareSerial.h>

boolean DEBUG = true;

#define rxPin 12
#define txPin 13

SoftwareSerial mySerial(rxPin, txPin); // RX, TX  
// Connect HM10      WeMoS D1
//     Pin 1/RXD          Pin D12
//     Pin 2/TXD          Pin D13


void setup() {
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input

  if (DEBUG) {
  Serial.begin(9600);
  }
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting");
  Serial.println(SSID);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IPess: ");
  Serial.println(WiFi.localIP());

  //  open software serial connection to the Bluetooth module.
  mySerial.begin(9600);
  if (DEBUG)  {   Serial.println("BTSerial started at 9600");
     }
  
  // If the baudrate of the HM-10 module has been updated,
  // you may need to change 9600 by another value
  // Once you have found the correct baudrate,
  // you can update it using AT+BAUDx command 
  // e.g. AT+BAUD0 for 9600 bauds

}
 
void loop(){
  if (mySerial.available() > 0) {
    c = mySerial.read();  
    Serial.print("Got input:");
    if (c != 0)
    {
      // Non-zero input means "turn on SECURITY".
      Serial.println("  on");
    }
    else
    {
      // Input value zero means "turn off SECURITY".
      Serial.println("  off");
    }  
  }

  
  var = digitalRead(inputPin);   // read input value of PIR
  if (var == HIGH) {             // check if motion is detected
    digitalWrite(ledPin, HIGH);  // turn LED ON
    timer = 0;                   // resets timer if motion is detected
    
    if (state == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // only print the output change
      state = HIGH;
    }

    if (timevar > emailinterval && c != 0){  // only print email if security is turned on
      timevar = 0;
      Serial.println("E-mail sent!");
      //byte ret = sendEmail();   // only sends email, if ~5 minutes have elapsed since last detection
    }
  }
  else {
    if (state == HIGH) {         // we have just turned off
      Serial.println("Motion ended!");
      state = LOW;               // only print the output change
      timer = 0;                 // resets timer when changing to no motion detected
    }
    if (timer > timerinterval) {
     digitalWrite(ledPin, LOW);  // turn LED OFF
    }
  }
  
  timevar++;                     // increment time variable
  timer++;                       // increment time variable
  delay(1);
}


byte sendEmail()
{
  byte thisByte = 0;
  byte respCode;

  if (client.connect(server, 2525) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!eRcv()) return 0;

  Serial.println(F("Sending EHLO"));
  client.println("EHLO www.example.com");
  if (!eRcv()) return 0;
  Serial.println(F("Sending auth login"));
  client.println("auth login");
  if (!eRcv()) return 0;
  Serial.println(F("Sending User"));
  // Change to your base64, ASCII encoded user
  client.println("dGhlaXNtdW5r"); // SMTP UserID
  if (!eRcv()) return 0;
  Serial.println(F("Sending Password"));
  // change to your base64, ASCII encoded password
  client.println("bWFQeFNVTXBhZTNt");//  SMTP Passw
     if (!eRcv()) return 0;
    Serial.println(F("Sending From"));   // change to your email address (sender)
   client.println(F("MAIL From: theis_mj@hotmail.com"));// not important 
   if (!eRcv()) return 0;   // change to recipient address
    Serial.println(F("Sending To"));
    client.println(F("RCPT To: theis_mj@hotmail.com"));
    if (!eRcv()) return 0;
    Serial.println(F("Sending DATA"));
    client.println(F("DATA"));
    if (!eRcv()) return 0;
    Serial.println(F("Sending email"));   // change to recipient address
   client.println(F("To: theis_mj@hotmail.com"));   // change to your address
   client.println(F("From: theis_mj@hotmail.com"));
 client.println(F("Subject: Status update\r\n"));
    client.println(F("There has been some movement."));
    client.println(F("."));
    if (!eRcv()) return 0;
    Serial.println(F("Sending QUIT"));
    client.println(F("QUIT"));
    if (!eRcv()) return 0;
    client.stop();
    Serial.println(F("disconnected"));
    return 1;
  }
  byte eRcv()
  {
    byte respCode;
    byte thisByte;
    int loopCount = 0;
    while (!client.available())
  {
      delay(1);
      loopCount++;     // if nothing received for 10 seconds, timeout
      if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  respCode = client.peek();
  while (client.available()) {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  if (respCode >= '4')
  {
    //  efail();
    return 0;
  }
  return 1;
}
