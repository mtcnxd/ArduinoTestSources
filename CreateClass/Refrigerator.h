class Refrigerator
{
    int defaultTemp = 170;
    int compressorPin = 0;
    int defrostPin = 2;

  public: String cooling(int temperature)
    {
      if (temperature > defaultTemp) {
        digitalWrite(compressorPin, HIGH);
        return "Cooling";
      } else {
        digitalWrite(compressorPin, LOW);
        return "Waiting";
      }
    }

  public: void makeDefrost(bool defrostStatus)
    {
      if (defrostStatus) {
        digitalWrite(defrostPin, HIGH);
      } else {
        digitalWrite(defrostPin, LOW);
      }
    }

  public: String run(int temperature, int runningTime)
    {
      if (runningTime > 15) {
        makeDefrost(true);
        return "Defrost";

      } else {
        makeDefrost(false);
        return cooling(temperature);

      }
    }

};
