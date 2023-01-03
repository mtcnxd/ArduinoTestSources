#include "Refrigerator.h";
int systemClock = 0;
static int pinDefrost = 0;
static int pinCompressor = 2;

Refrigerator fridge;

void setup()
{
  Serial.begin(9600);
  pinMode(pinDefrost, OUTPUT);
  pinMode(pinCompressor, OUTPUT);
  Serial.println("Iniciando codigo");
}

void loop()
{
  int sensorValue = analogRead(A0);
  String fridgeStatus = fridge.run(sensorValue, systemClock);
  Serial.println("Estado: " + fridgeStatus + " Temperature: " + (String) sensorValue);

  if (systemClock <= 30) {
    systemClock++;
    delay(1000);
  } else {
    systemClock = 0;
  }

}
