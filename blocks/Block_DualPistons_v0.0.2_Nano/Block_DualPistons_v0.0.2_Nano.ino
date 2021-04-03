#include <Wire.h>
#include <probatio_defs.h>

//these pins can not be changed 2/3 are special pins
int address = BLOCK_DUAL_PISTONS_1; //40
int dataSize = SIZE_BLOCK_DUAL_PISTONS_1; //2
byte value[SIZE_BLOCK_DUAL_PISTONS_1];
int pinButton[] = {A0, A1};

void setup() {
  Wire.begin(address);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < dataSize; i++) {
    int valueAnalog = analogRead(pinButton[i]);
    int mappedValue = map(valueAnalog, 0, 1024, 0, 255);
    int constrainedValue = constrain(mappedValue, 0, 255);
    value[i] = constrainedValue;
    Serial.println(value[i]);
  }
}

void requestEvent(){
  Wire.write(value, dataSize);
}
