#include <ESP32Servo.h>

class aktuator{
  private:
    int pos;
    int servoPin;
    
    Servo servo;
  public:
    int PompaPin;
    int LedPin;
    int FanPin;
    aktuator(int pinservo, int pinpompa, int ledpin, int fanpin) : servoPin(pinservo), PompaPin(pinpompa), LedPin(ledpin), FanPin(fanpin){
      servo.attach(servoPin);
      pinMode(PompaPin, OUTPUT);
      pinMode(LedPin, OUTPUT);
      pinMode(FanPin, OUTPUT);
    }
  
  void ServoPos(int angle){
    servo.write(angle);
  }
};