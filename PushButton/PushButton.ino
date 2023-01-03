#include "PushButtonClass.h";

int pinTest  = 16;
int counter = 0;

bool pressed = false;
Button button1(14);

void setup() {
  Serial.begin(9600);
  pinMode(pinTest, INPUT_PULLUP);
}

void loop() {
  bool pushStatus = digitalRead(pinTest);

  button1.isPressed();

  if (pushStatus && !pressed) {
    counter++;
    Serial.println("Pressed: " + (String) counter);
    pressed = true;
  } else if (!pushStatus && pressed) {
    pressed = false;
  }

  delay(25);
}
