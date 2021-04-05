
#include "AuxFunctions.h"
#include <avr/wdt.h>
#include <Bounce2.h>

#define BUTTON_PIN 7
#define LED_CONNECTED 9
#define LED_MAPPING 5

int threshold = 1;
boolean isMapping = true;
boolean isFirstTime = true;
boolean hasAtLeastABlockConnected = false;

Bounce bt = Bounce();

void setup() {
  wdt_disable();

  /*
      ==============
      Board Setup
      ==============
  */
  pinMode(LED_CONNECTED, OUTPUT);
  pinMode(LED_MAPPING, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  bt.attach(BUTTON_PIN);
  bt.interval(5);

  /*
      ==============
      Communication Setup
      ==============
  */

  Wire.begin();
  Serial.begin(115200);

  /*
     ==============
     Watchdog Setup
     ==============
  */

  wdt_enable(WDTO_120MS);
  //  wdt_enable(WDTO_1S);
}

void loop() {

  /*
    ==============
    Mapping Button
    ==============
  */

  bt.update();

  int value = bt.read();

  if (!isFirstTime) {
    if (value == LOW) {
      isMapping = true;
    } else {
      isMapping = false;
    }
  } else {
    if(bt.fell()){
      isFirstTime = false;
    }
  }

  if (isMapping) {
    threshold = 3;
  } else {
    threshold = 1;
  }

  digitalWrite(LED_MAPPING, isMapping);
  digitalWrite(LED_CONNECTED, hasAtLeastABlockConnected);

  /*
    =================
    I2C Data Fetching
    =================
  */

  requestI2C();
  formatBufferWithBlocks();

  /*
    =======
    Actions
    =======
  */

  //  sendConsolidatedSerialMessage();
  hasAtLeastABlockConnected = sendIndividualMIDIMessages(threshold);
  //debugDumpBuffer();
  //sendIndividualOSCMessages();
  // sendConsolidatedOSCMessage();

  wdt_reset();
}
