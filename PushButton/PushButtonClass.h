class Button {
  private:
    byte pin;
    byte state;

  public:
    Button(byte pin) {
      this->pin = pin;
      pinMode(pin, INPUT_PULLUP);
    }

    void update() {
      byte newReading = digitalRead(pin);
      delay(20);

      if (newReading && !state) {
        Serial.println("Apretado");
        state = true;
      } else if (!newReading && state) {
        state = false;
      }

    }

    bool isPressed() {
      update();
      return state;
    }
};
