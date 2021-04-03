#include <Wire.h>
#include <probatio_defs.h>

int address_ = BLOCK_DUAL_BUTTONS_1;
int dataSize = SIZE_BLOCK_DUAL_BUTTONS_1;
byte value[SIZE_BLOCK_DUAL_BUTTONS_1];

boolean buttonStatus[2];
int pinButton[] = {10, 11};
int counter = 0;

void setup()
{
  Wire.begin(address_);      // init I2C Slave mode
  Wire.onRequest(requestEvent);
  for (int i = 0; i < dataSize; i++) {
    pinMode(pinButton[i], INPUT_PULLUP);
  }
  Serial.begin(9600);

}

void loop()
{
  for (int i = 0; i < dataSize; i++) {
    if (digitalRead(pinButton[i]) == LOW) {
      value[i] = 255;
      Serial.println("apertou");
    } else {
      value[i] = 0;
      Serial.println("soltou");
    }
  }
}

void requestEvent() {
  Wire.write(value, dataSize);
}
