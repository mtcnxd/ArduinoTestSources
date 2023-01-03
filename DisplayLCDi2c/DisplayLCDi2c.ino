#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int ledPin = LED_BUILTIN;
int counter = 0;

void setup() {
  lcd.init();
  Wire.begin(2, 0);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Counter ");
  lcd.print(counter);
  Serial.println("Esta es una prueba: " + (String) counter);
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
  counter++;
}
