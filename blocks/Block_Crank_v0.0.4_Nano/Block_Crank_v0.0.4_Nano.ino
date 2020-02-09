/*
Report April 28 2015 IDMIL

Clockwise -> PIN CLK is PIN 2
DT -> PIN DT is PIN 3
Each step is adding the value 4;
Total steps is value 80;
*/

#include <Wire.h>
#include <probatio_defs.h>

int address_ = BLOCK_CRANK;
int dataSize = SIZE_BLOCK_CRANK;
byte value[SIZE_BLOCK_CRANK];

//these pins can not be changed 2/3 are special pins
int encoderPin1 = 2;
int encoderPin2 = 3;
int encoderSwitchPin = 5; //push button switch

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastEncoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;
byte valueRotary = 0;
byte valueButton = 0;

int MSB = 0;
int LSB = 0;

void setup() {
  delay(100);
  
  //Serial.begin (9600);

  Wire.begin(address_);
  Wire.onRequest(requestEvent);
  
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);

  pinMode(encoderSwitchPin, INPUT_PULLUP);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  //digitalWrite(encoderSwitchPin, HIGH); //turn pullup resistor on


  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);

}

void loop() {
  //Do stuff here
  if (digitalRead(encoderSwitchPin)) {
    valueButton = 0;
  }
  else {
    valueButton = 255;
  }


  if (lastEncoderValue != encoderValue) {
    //Serial.write(encoderValue);
    //    Serial.print(MSB);
    //    Serial.print(" ");
    //    Serial.println(LSB);
  }
  //delay(1000); //just here to slow down the output, and show it will work  even during a delay
  lastEncoderValue = encoderValue;
  valueRotary = map(encoderValue, 0, 79, 0, 255);
  //delay(50);
}


void updateEncoder() {
  MSB = digitalRead(encoderPin1); //MSB = most significant bit
  LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    encoderValue ++;
    if (encoderValue == 79) {
      encoderValue = 79;
    }
    if (encoderValue == 80) {
      encoderValue = 0;
    }
  }
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    encoderValue --;
    if (encoderValue == -1) {
      encoderValue = 79;
    }
    if (encoderValue == -2) {
      encoderValue = 78;
    }
  }



  lastEncoded = encoded; //store this value for next time

  //Serial.println(encoderValue);
}

void requestEvent(){
  byte data[] = {valueRotary, valueButton};
  Wire.write(data, dataSize);
}
