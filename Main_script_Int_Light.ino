/*
 * Main script
 */

/*
 * Kode til indstilling af ur er inspireret af: https://www.geekstips.com/arduino-time-sync-ntp-server-esp8266-udp/
 */

/* Import libraries */
#include <ESP8266WiFi.h>  
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/* Funcion declarations*/
unsigned long sendNTPpacket(IPAddress& address);    // function prototype
byte sendEmail();                                   // function prototype
byte eRcv();                                        // function prototype

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/* Don't hardwire the IP address or we won't get the benefits of the pool.
 *  Lookup the IP address for the host name instead */
IPAddress timeServerIP;  // time.nist.gov NTP server address
const char* ntpServerName = "2.dk.pool.ntp.org";


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/* Definitions*/
// For time keeping
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
unsigned int localPort = 2390;    // local port to listen for UDP packets

// Motion sensor
int ledPin = D10;                 // pin for LED
int inputPin = D13;               // pin for PIR sensor
int state = LOW;                  // initially no motion detected 
int var = 0;                      // variable for reading the pin status
int emailinterval = 150;          // time interval between e-mails sent (seconds)
int timerinterval = 5;            // time interval from no motion detected to light is turned off (seconds)
unsigned long starttime;          // epoch at time of detected movement OR change to no movement
unsigned long timevar;            // epoch at time of last e-mail sent

// BLE HM-10
int c;            // variable to store security setting
boolean DEBUG = true;
#define rxPin 12
#define txPin 13

SoftwareSerial mySerial(rxPin, txPin); // RX, TX  
// Connect HM10        WeMoS D1
//     Pin 1/RXD          Pin D12
//     Pin 2/TXD          Pin D13

// WIFI
// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
// SSID and password of WiFi network
//const char* SSID = "DTU-Networks";
//const char* PASS = "Fotonik343";
const char* SSID = "111";
const char* PASS = "sokrates";
char server[] = "mail.smtp2go.com";
ADC_MODE(ADC_VCC);

WiFiClient client;


void setup() {
  pinMode(ledPin, OUTPUT);      // declare LED/Light bulb as output
  pinMode(txPin, OUTPUT);       // declare transmit pin as output
  pinMode(inputPin, INPUT);     // declare sensor as input

  if (DEBUG) {
  Serial.begin(9600);
  }

  // Connect to WiFi
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting to ");
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

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);

    // Set GMT+1 time
    setTime(epoch + 3600);
  }

  
  //  open software serial connection to the Bluetooth module.
  mySerial.begin(9600);
  if (DEBUG) {   
    Serial.println("BTSerial started at 9600");
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
    Serial.print("Security:");
    if (c != 0)
    {
      // Non-zero input means "turn on SECURITY".
      Serial.println("  ON");
    }
    else
    {
      // Input value zero means "turn off SECURITY".
      Serial.println("  OFF");
    }  
  }


  var = digitalRead(inputPin);   // read input value of PIR
  if (var == HIGH) {             // check if motion is detected
    digitalWrite(ledPin, HIGH);  // turn LED ON
    starttime = now();           // sets time if motion is detected
    
    if (state == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // only print the output change
      state = HIGH;
    }

    if (now() - timevar > emailinterval && c != 0){  // only print email if security is turned on
      timevar = now();
      Serial.println("E-mail sent!");
      byte ret = sendEmail();   // only sends email, if ~5 minutes have elapsed since last detection
    }
  }
  else {
    if (state == HIGH) {         // we have just turned off
      Serial.println("Motion ended!");
      state = LOW;               // only print the output change
      starttime = now();                 // resets timer when changing to no motion detected
    }
    if (now() - starttime > timerinterval) {
     digitalWrite(ledPin, LOW);  // turn LED OFF
    }
  }
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
    client.println(F("Subject: Home intrusion alert\r\n"));
    client.println(F("There has been some movement."));
    client.print(day());
    client.print(F("-"));
    client.print(month());
    client.print(F("-"));
    client.print(year());
    client.print(F(" at "));
    client.print(hour());
    client.print(F(":"));
    client.print(minute());
    client.print(F(":"));
    client.print(second());
    client.println(F("."));
    client.println(F("."));
    if (!eRcv()) return 0;
    Serial.println(F("Sending QUIT"));
    client.println(F("QUIT"));
    if (!eRcv()) return 0;
    client.stop();
    Serial.println(F("disconnected"));
    return 1;
  }
  byte eRcv() {
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

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
