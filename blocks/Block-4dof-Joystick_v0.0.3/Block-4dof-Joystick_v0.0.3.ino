#include "MIDIUSB.h"
#include <Bounce2.h>
#include "FilterPot.h"
#include <Wire.h>
#include <probatio_defs.h>

Bounce button = Bounce();
FilterPot filteredX = FilterPot(SEVEN_BIT_READING, 0.8);
FilterPot filteredY = FilterPot(SEVEN_BIT_READING, 0.8);
FilterPot filteredR = FilterPot(SEVEN_BIT_READING, 0.8);

int triggeredNote = 0;

int address_ = BLOCK_DOF4_JOYSTICK_2;
int dataSize = SIZE_BLOCK_DOF4_JOYSTICK_2;
byte value[SIZE_BLOCK_DOF4_JOYSTICK_2];

void setup() {
  //  Serial.begin(115200);
  Wire.begin(address_);      // init I2C Slave mode
  Wire.onRequest(requestEvent);
  pinMode(4, INPUT_PULLUP);
  button.attach(4);
  button.interval(10);
}

void loop() {
  button.update();

  int readX = analogRead(A0);
  int readY = analogRead(A1);
  int readR = analogRead(A2);

  value[0] = map(readX, 0, 1024, 0, 255);
  value[1] = map(readY, 0, 1024, 0, 255);
  value[2] = map(readR, 0, 1024, 0, 255);

  if (button.read() == LOW) {
    value[3] = 255;
  } else {
    value[3] = 0;
  }

  filteredX.updateValue(readX);
  filteredY.updateValue(readY);
  filteredR.updateValue(readR);

  //  int x = map(analogRead(A0), 0, 1023, 0, 127);
  //  int y = map(analogRead(A1), 0, 1023, 0, 127);
  //  int r = map(analogRead(A2), 0, 1023, 0, 127);

  if (button.fell()) {
    triggeredNote = 44;//map(filteredX.getFilteredValue(), 0, 127, 40, 60);
    noteOn(0, triggeredNote, 120);
    MidiUSB.flush();
  }
  if (button.rose()) {
    noteOff(0, triggeredNote, 120);
    MidiUSB.flush();
  }

  if (filteredX.hasValueChanged()) {
    controlChange(0, 35, filteredX.getFilteredValue());
    MidiUSB.flush();
  }

  if (filteredY.hasValueChanged()) {
    controlChange(0, 36, filteredY.getFilteredValue());
    MidiUSB.flush();
  }

  if (filteredR.hasValueChanged()) {
    controlChange(0, 37, filteredR.getFilteredValue());
    MidiUSB.flush();
  }

  //  Serial.println("Sending note on");
  //  noteOn(0, 48, 64);   // Channel 0, middle C, normal velocity
  //  MidiUSB.flush();
  //  delay(500);
  //  Serial.println("Sending note off");
  //  noteOff(0, 48, 64);  // Channel 0, middle C, normal velocity
  //  MidiUSB.flush();
  //  delay(1500);

  // controlChange(0, 10, 65); // Set the value of controller 10 on channel 0 to 65
}

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void requestEvent() {
  Wire.write(value, dataSize);
}
