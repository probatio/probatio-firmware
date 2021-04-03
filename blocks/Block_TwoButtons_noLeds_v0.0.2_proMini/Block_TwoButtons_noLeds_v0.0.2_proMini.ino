#include <Wire.h>
#include <probatio_defs.h>
#include <Bounce2.h>

int address_ = BLOCK_DUAL_BUTTONS_1; //address 16
int dataSize = SIZE_BLOCK_DUAL_BUTTONS_1; //2
byte value[SIZE_BLOCK_DUAL_BUTTONS_1];

#define NUM_BUTTONS 2
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {10, 11};

Bounce * buttons = new Bounce[NUM_BUTTONS];

boolean buttonStatus[2];

void setup()
{
  Wire.begin(address_);      // init I2C Slave mode
  Wire.onRequest(requestEvent);
  //  for (int i = 0; i < dataSize; i++) {
  //    pinMode(pinButton[i], INPUT_PULLUP);
  //  }
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach( BUTTON_PINS[i] , INPUT_PULLUP  );       //setup the bounce instance for the current button
    buttons[i].interval(25);              // interval in ms
  }
//  Serial.begin(9600);

}

void loop()
{
  for (int i = 0; i < NUM_BUTTONS; i++)  {
    buttons[i].update();
    if ( buttons[i].fell() ) {
      value[i] = 255;
      digitalWrite(LED_BUILTIN, HIGH);
//      Serial.print("Button "); Serial.print(i); Serial.print(": "); Serial.println(value[i]);
    }
    else if ( buttons[i].rose() ) {
      value[i] = 0;
      digitalWrite(LED_BUILTIN, LOW);
//      Serial.print("Button "); Serial.print(i); Serial.print(": "); Serial.println(value[i]);
    }
  }
}

void requestEvent() {
  Wire.write(value, dataSize);
}
