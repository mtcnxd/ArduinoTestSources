#include <Servo.h>

bool isPushPressed = false;
bool ledStatus = false;
int pinPush = 5;
int counter = 0;
int pinLed  = 4;

Servo myservo;

int position = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pinPush, INPUT_PULLUP);
  pinMode(pinLed, OUTPUT);
  myservo.attach(0);
}

void loop() 
{
  bool pushButton = !digitalRead(pinPush);
  delay(100);

  //toggleOutput (pushButton);

  if (pushButtonPressed(pushButton)) {
    Serial.println("Counter: " + (String) counter);
    myservo.write(counter);
    counter += 1;
  }

}
