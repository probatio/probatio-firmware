#include <Wire.h>
#include <probatio_defs.h>

//these pins can not be changed 2/3 are special pins
int address = BLOCK_TAP_1;
int dataSize = SIZE_BLOCK_TAP_1;
byte value[SIZE_BLOCK_TAP_1];
int pinButton[] = {A0};
int filteredValue[1] = { 0 };

int peakDetection(int sensorVal, int thresholdVal = 1);

int maxVal = 0;
double lastTriggerTime = 0;

int debounceTime = 30;
int thresholdVal = 50; //out of 1023

void setup() {
  Wire.begin(address);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
}


void loop() {
  for (int i = 0; i < dataSize; i++) {
    int valueAnalog = analogRead(pinButton[i]);
    filteredValue[i] = EMA(valueAnalog, filteredValue[i], 0.5);
    int peakValue = peakDetection(filteredValue[i], thresholdVal, debounceTime);
    int mappedValue = map(peakValue, 200, 1023, 0, 255);
    int constrainedValue = constrain(mappedValue, 0, 255);

    if (millis() - lastTriggerTime < 200) {
      if (constrainedValue > 0) value[i] = constrainedValue;
    } else{
      value[i] = 0;
    }
    Serial.print(value[i] * 4); Serial.print(", "); 
    Serial.print(valueAnalog); Serial.print(", "); 
    Serial.print(filteredValue[i]); Serial.print(", "); 
    Serial.print(thresholdVal); Serial.print(", "); 
    Serial.println();
  }

}

void requestEvent() {
  Wire.write(value, dataSize);
}


int peakDetection(int sensorVal, int thresholdVal, int debounceTime_) {
  if (millis() - lastTriggerTime > debounceTime_) {
    if (sensorVal > thresholdVal) {
      if (sensorVal > maxVal) {
        maxVal = sensorVal;
      } else {
        int output = maxVal;
        maxVal = 0;
        lastTriggerTime = millis();
        //Serial.println(output);
        return output;
      }
    }
  }
  return 0;
}

int EMA(int inputValue, int filteredValue, float filteringAmount) {
  return ((1 - filteringAmount) * inputValue) + (filteringAmount * filteredValue);
}
