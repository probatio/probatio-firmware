#include <Wire.h>
#include <probatio_defs.h>

int address_ = BLOCK_TWOBUTTONS1;
int dataSize = SIZE_BLOCK_TWOBUTTONS1;
byte value[SIZE_BLOCK_TWOBUTTONS1];

boolean buttonStatus[2];
int pinButton[] = {2, 3};
int pinLEDs[] = {5, 6};
int counter = 0;

void setup()
{
  Wire.begin(address_);      // init I2C Slave mode
  Wire.onRequest(requestEvent);
  for (int i = 0; i < dataSize; i++) {
    pinMode(pinButton[i], INPUT_PULLUP);
  }
  for (int i = 0; i < dataSize; i++) {
    pinMode(pinLEDs[i], OUTPUT);
  }
}

void loop()
{
  for (int i = 0; i < dataSize; i++) {
    if (digitalRead(pinButton[i]) == LOW) {
      value[i] = 255;
      digitalWrite(pinLEDs[i], HIGH);
    } else {
      value[i] = 0;
      digitalWrite(pinLEDs[i], LOW);
    }
  }
}

void requestEvent() {
  Wire.write(value, dataSize);
}
