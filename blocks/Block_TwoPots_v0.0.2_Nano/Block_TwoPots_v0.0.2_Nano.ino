#include <Wire.h>
#include <probatio_defs.h>

//these pins can not be changed 2/3 are special pins
int address = BLOCK_TWOPOTS;
int dataSize = SIZE_BLOCK_TWOPOTS;
byte value[SIZE_BLOCK_TWOPOTS];
int pinButton[] = {A0, A1};

void setup() {
  Wire.begin(address);
  Wire.onRequest(requestEvent);
}

void loop() {
  for (int i = 0; i < dataSize; i++) {
    int valueAnalog = analogRead(pinButton[i]);
    int mappedValue = map(valueAnalog, 0, 1024, 0, 255);
    int constrainedValue = constrain(mappedValue, 0, 255);
    value[i] = constrainedValue;
  }
}

void requestEvent(){
  Wire.write(value, dataSize);
}
