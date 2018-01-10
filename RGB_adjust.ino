int ledPin1 = 3; 
int ledPin2 = 5; 
int ledPin3 = 6; 
int maxOutputLed = 100;
int delayTime = 6000;

int ledRedOutput = 1;
int ledGreenOutput = 1;
int ledBlueOutput = 1;

void setup() {
  // put your setup code here, to run once:

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  analogWrite(ledPin1, ledRedOutput);
  analogWrite(ledPin2, ledGreenOutput);
  analogWrite(ledPin3, ledBlueOutput);

  

}
