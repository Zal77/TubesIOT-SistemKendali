#include "controller.h"

void setup(){
  Serial.begin(115200);
  server_setup();
  //Servo();
}

void loop(){
  getDataAll();
}
