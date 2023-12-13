#include <DHT.h>

class dhtsensor{
  private:
    int pinDht;
    DHT dht;

  public:
    dhtsensor(int sensorPin) : pinDht(sensorPin), dht(sensorPin, DHT11){
      pinMode(pinDht, INPUT);
      dht.begin();
    }

  float bacasuhu(){
    return dht.readTemperature();
  }

  float bacakelembaban(){
    return dht.readHumidity();
  }
};