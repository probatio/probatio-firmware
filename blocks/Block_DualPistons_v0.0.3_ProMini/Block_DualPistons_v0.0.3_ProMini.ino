#include <Wire.h>
#include <probatio_defs.h>

//these pins can not be changed 2/3 are special pins
int address = BLOCK_DUAL_PISTONS_1; //40
int dataSize = SIZE_BLOCK_DUAL_PISTONS_1; //2
byte value[SIZE_BLOCK_DUAL_PISTONS_1];
int pinButton[] = {A0, A1};

int filteredValue[2] = {0, 0};

void setup() {
  Wire.begin(address);
  Wire.onRequest(requestEvent);
//  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < dataSize; i++) {
    int valueAnalog = analogRead(pinButton[i]);
    filteredValue[i] = EMA(valueAnalog, filteredValue[i], 0.6);
//    Serial.print(filteredValue[i]); Serial.print(" ");
    int mappedValue, constrainedValue;
    if (i == 0) {
      mappedValue = map(filteredValue[i], 543, 877, 0, 255); //calibration made with actual sensor values
      constrainedValue = constrain(mappedValue, 0, 255);
    }
    else if (i == 1) {
      mappedValue = map(filteredValue[i], 553, 877, 0, 255);
      constrainedValue = constrain(mappedValue, 0, 255);
    }
    value[i] = constrainedValue;
//    Serial.print(value[i]); Serial.print(" ");
  }
//  Serial.println();

}


int EMA(int inputValue, int filteredValue, float filteringAmount) {
  return ((1 - filteringAmount) * inputValue) + (filteringAmount * filteredValue);
}
void requestEvent() {
  Wire.write(value, dataSize);
}
