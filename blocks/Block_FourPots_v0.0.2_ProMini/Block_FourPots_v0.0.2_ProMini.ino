#include <Wire.h>
#include <probatio_defs.h>

int address = BLOCK_QUAD_POTS_2; // address 45
int dataSize = SIZE_BLOCK_QUAD_POTS_2; // 4 values
byte value[SIZE_BLOCK_QUAD_POTS_2];
int pinButton[] = {A0, A1, A2, A3};

int filteredValue[4] = {0, 0, 0, 0};
int lastValue[4] = {0, 0, 0, 0};

void setup() {
  Wire.begin(address);
  Wire.onRequest(requestEvent);
  for (int i = 0; i < dataSize ; i++) {
    pinMode(pinButton[i], INPUT);
  }
//  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < dataSize; i++) {
    int valueAnalog = analogRead(pinButton[i]);
    filteredValue[i] = EMA(valueAnalog, filteredValue[i], 0.6);
    int mappedValue = filteredValue[i] / 4; //map from 0 to 255
    int constrainedValue = constrain(mappedValue, 0, 255);
    value[i] = constrainedValue;
//    if (abs(value[i] - lastValue[i]) >= 1){
//    lastValue[i] = value[i];
//      Serial.print("pot "); Serial.print(i); Serial.print(": "); Serial.println(value[i]);
//    }
  }
}

void requestEvent() {
  Wire.write(value, dataSize);
}

int EMA(int inputValue, int filteredValue, float filteringAmount) {
  return ((1 - filteringAmount) * inputValue) + (filteringAmount * filteredValue);
}
