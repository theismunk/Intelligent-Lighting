int ledPin1 = 3;
int ledPin2 = 5;
int ledPin3 = 6; 

//void doMode(int mode) { 

int ledRedOutput = 0;
int ledGreenOutput = 0;
int ledBlueOutput = 0;

//}

int maxOutputLed = 0;
long delayTime = 120000;


void setup() {
  // put your setup code here, to run once:

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

  switch(4) {

  case 1:  //red
    ledRedOutput = maxOutputLed;
    ledGreenOutput = 0;
    ledBlueOutput = 0;

    break;

  case 2:  //green
    ledRedOutput = 0;
    ledGreenOutput = maxOutputLed;
    ledBlueOutput = 0;

    break;

  case 3: //blue
    ledRedOutput = 0;
    ledGreenOutput = 0;
    ledBlueOutput = maxOutputLed;

    break;

  case 4: //white
    ledRedOutput = maxOutputLed;
    ledGreenOutput = maxOutputLed;
    ledBlueOutput = maxOutputLed;

    break;

  case 5: //yellow
    ledRedOutput = maxOutputLed;
    ledGreenOutput = maxOutputLed;
    ledBlueOutput = 0;

    break;

  case 6: //turquoise
    ledRedOutput = 0;
    ledGreenOutput = maxOutputLed;
    ledBlueOutput = maxOutputLed;

    break;

  case 7: //violet
    ledRedOutput = maxOutputLed;
    ledGreenOutput = 0;
    ledBlueOutput = maxOutputLed;

    break;
    
  case 8: //purple
    ledRedOutput = maxOutputLed / 2;
    ledGreenOutput = 0;
    ledBlueOutput = maxOutputLed;

    break;

  case 9: //pink
    ledRedOutput = maxOutputLed;
    ledGreenOutput = 0;
    ledBlueOutput = maxOutputLed / 2;

    break;

  case 10: //orange
    ledRedOutput = maxOutputLed;
    ledGreenOutput = maxOutputLed / 2;
    ledBlueOutput = 0;

    break;  
    
  }

    for (int i = 0; i <= maxOutputLed; i++) {
      
      analogWrite(ledPin1, i * ledRedOutput / maxOutputLed);
      analogWrite(ledPin2, i * ledGreenOutput / maxOutputLed);
      analogWrite(ledPin3, i * ledBlueOutput / maxOutputLed);

      //Serial.println(i);

      delay(delayTime / maxOutputLed);

      if (i == maxOutputLed) {
    
      while(true) {
      
      analogWrite(ledPin1, maxOutputLed * ledRedOutput / maxOutputLed);
      analogWrite(ledPin2, maxOutputLed * ledGreenOutput / maxOutputLed);
      analogWrite(ledPin3, maxOutputLed * ledBlueOutput / maxOutputLed);
        }
      }

      /* if(break command) {
       *   break;
       * 
       */
    }


    /*if (i == maxOutputLed)
    
      while(true) {
      
      analogWrite(ledPin, maxOutputLed); }

  } */

}
