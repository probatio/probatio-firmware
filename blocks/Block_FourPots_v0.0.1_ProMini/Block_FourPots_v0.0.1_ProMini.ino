#include <Wire.h>
#include <probatio_defs.h>

int address = BLOCK_QUAD_POTS_1; // address 44
int dataSize = SIZE_BLOCK_QUAD_POTS_1; // 4 values
byte value[SIZE_BLOCK_QUAD_POTS_1];
int pinButton[] = {A0, A1, A2, A3};

void setup() {
  Wire.begin(address);
  Wire.onRequest(requestEvent);
  for (int i=0; i< dataSize ; i++){
    pinMode(pinButton[i], INPUT);
  }
  Serial.begin(115200);
}

void loop() {
  for (int i = 0; i < dataSize; i++) {
    int valueAnalog = analogRead(pinButton[i]);
    int mappedValue = valueAnalog / 4; //map from 0 to 255
    int constrainedValue = constrain(mappedValue, 0, 255);
    value[i] = constrainedValue;
    Serial.print(value[i]); Serial.print(" | ");
  }
  Serial.println();
}

void requestEvent(){
  Wire.write(value, dataSize);
}
