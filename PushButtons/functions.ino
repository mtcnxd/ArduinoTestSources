/*
  FUNCTIONS BLOCK
*/

bool toggleOutput(bool pushButton)
{
  if (pushButton) {
    if (!isPushPressed) {
      if (ledStatus) {
        ledStatus = false;
        Serial.println("LED Status: " + (String) ledStatus);

      } else {
        ledStatus = true;
        Serial.println("LED Status: " + (String) ledStatus);
      }
    }
    isPushPressed = true;

  } else {
    isPushPressed = false;
  }

  return ledStatus;
}


bool pushButtonPressed(bool pushButton)
{
  bool buttonPressed = false;
  if (pushButton) {
    if (!isPushPressed) {
      buttonPressed = true;
    }
    isPushPressed = true;

  } else {
    isPushPressed = false;
  }
  
  return buttonPressed;
}
