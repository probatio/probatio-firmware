/*
Based on the Rotary Library by buxtronix: 
http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html

Clockwise -> PIN CLK is PIN 2
DT -> PIN DT is PIN 3
*/

#include <Wire.h>
#include <probatio_defs.h>
#include <Rotary.h>


int address_ = BLOCK_ROTARY_2; //address 65
int dataSize = SIZE_BLOCK_ROTARY_2; //2
byte value[SIZE_BLOCK_ROTARY_2];

//these pins can not be changed 2/3 are special pins
// Rotary encoder is wired with the common to ground and the two
// outputs to pins 2 and 3.
Rotary rotary = Rotary(2, 3); //(CLK, DT)

int encoderSwitchPin = 5; //push button switch



long lastEncoderValue = 0;

byte valueRotary = 0;
byte valueButton = 0;

int counter = 0;

void setup() {
  Wire.begin(address_);
  Wire.onRequest(requestEvent);
  
  Serial.begin (9600);
  
  attachInterrupt(0, rotate, CHANGE);
  attachInterrupt(1, rotate, CHANGE);

  pinMode(encoderSwitchPin, INPUT_PULLUP);

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(0, rotate, CHANGE);
  attachInterrupt(1, rotate, CHANGE);

}

void loop() {
  //Do stuff here
  if (digitalRead(encoderSwitchPin)) {
    valueButton = 0;
  }
  else {
    valueButton = 255;
  }
}

// rotate is called anytime the rotary inputs change state.
void rotate() {
  unsigned char result = rotary.process();
  if (result == DIR_CW) {
    valueRotary--;
    if (valueRotary < 0) valueRotary += 256;
    Serial.println(valueRotary);
  } else if (result == DIR_CCW) {
    valueRotary++;
    if (valueRotary > 255) valueRotary -= 256;
    Serial.println(valueRotary);
  }
}

void requestEvent(){
  byte data[] = {valueRotary, valueButton};
  Wire.write(data, dataSize);
}
