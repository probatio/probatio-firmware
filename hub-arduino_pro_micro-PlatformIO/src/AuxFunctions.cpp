#include "AuxFunctions.h"

int cycle = 0;

byte buffer[BUFFER_SIZE];
int indexBuffer = 0;

bool loopCycleControl = true;
int loopCycleTime = 100;

int quantBlocksConnected = 0;

bool isConnected = false;

//char c_dual_buttons_1[] = "dual_buttons_1";
//char c_obsf_24[] = "obsf_24";
//char c_obsf_30[] = "obsf_30";
//char c_cherries_1[] = "cherries_1";
//char c_dual_pots_1[] = "dual_pots_1";
//char c_tap_1[] = "tap_1";
//char c_tap_2[] = "tap_2";
//char c_dual_pistons_1[] = "dual_pistons_1";
//char c_joystick_1[] = "joystick_1";
//char c_four_pistons_1[] = "four_pistons_1";
//char c_four_thumbs_1[] = "four_thumbs_1";
//char c_dof4_joystick_1[] = "dof4_joystick_1";
//char c_dof4_joystick_2[] = "dof4_joystick_2";
//char c_crank_1[] = "crank_1";
//char c_dial_1[] = "dial_1";
//char c_debug[] = "debug";

Block dual_buttons_1(BLOCK_DUAL_BUTTONS_1, SIZE_BLOCK_DUAL_BUTTONS_1, 0, 10, "dual_buttons_1");
Block obsf_24(BLOCK_OBSF_24, SIZE_BLOCK_OBSF_24, 0, 12, "obsf_24");
Block obsf_30(BLOCK_OBSF_30, SIZE_BLOCK_OBSF_30, 0, 13, "obsf_30");
Block cherries_1(BLOCK_CHERRIES_1, SIZE_BLOCK_CHERRIES_1, 0, 14, "cherries_1");
Block dual_pots_1(BLOCK_DUAL_POTS_1, SIZE_BLOCK_DUAL_POTS_1, 0, 18, "dual_pots_1");
Block tap_1(BLOCK_TAP_1, SIZE_BLOCK_TAP_1, 0, 20, "tap_1");
Block tap_2(BLOCK_TAP_2, SIZE_BLOCK_TAP_2, 0, 21, "tap_2");
Block dual_pistons_1(BLOCK_DUAL_PISTONS_1, SIZE_BLOCK_DUAL_PISTONS_1, 0, 22, "dual_pistons_1");
Block joystick_1(BLOCK_JOYSTICK_1, SIZE_BLOCK_JOYSTICK_1, 0, 24, "joystick_1");
Block four_pistons_1(BLOCK_FOUR_PISTONS_1, SIZE_BLOCK_FOUR_PISTONS_1, 0, 27, "four_pistons_1");
Block four_thumbs_1(BLOCK_FOUR_THUMBS_1, SIZE_BLOCK_FOUR_THUMBS_1, 0, 31, "four_thumbs_1");
Block dof4_joystick_1(BLOCK_DOF4_JOYSTICK_1, SIZE_BLOCK_DOF4_JOYSTICK_1, 0, 35, "dof4_joystick_1");
Block dof4_joystick_2(BLOCK_DOF4_JOYSTICK_2, SIZE_BLOCK_DOF4_JOYSTICK_2, 0, 39, "dof4_joystick_2");
Block crank_1(BLOCK_CRANK_1, SIZE_BLOCK_CRANK_1, 0, 43, "crank_1");
Block dial_1(BLOCK_DIAL_1, SIZE_BLOCK_DIAL_1, 0, 45, "dial_1");
Block debug_(BLOCK_DEBUG, SIZE_BLOCK_DEBUG, 0, 47, "debug");

Block* blocks[QUANTITY_BLOCKS] = {&dual_buttons_1, &obsf_24, &obsf_30, &cherries_1, &dual_pots_1, &tap_1, &tap_2, &dual_pistons_1, &joystick_1, &four_pistons_1, &four_thumbs_1, &dof4_joystick_1, &dof4_joystick_2, &crank_1, &dial_1, &debug_};

/*
   =============
   I2C data requisition to blocks
   =============
*/

void requestI2C() {
  quantBlocksConnected = 0;
  for (int i = 0; i < QUANTITY_BLOCKS; i++) {
    bool isConnected = requestFromDevice(blocks[i]);
    if (isConnected) {
      quantBlocksConnected++;
    }
  }
}

bool requestFromDevice(Block * _block) {
  bool result = false;
  Wire.requestFrom((uint8_t)_block->id, (uint8_t)_block->quantity, (uint8_t)true);
  if (Wire.available())
  {
    _block->isConnected = true;
    result = true;
    for (int i = 0; i < _block->quantity; i++) {
      _block->lastValues[i].setValue(_block->values[i].getValue());
      _block->values[i].setValue(Wire.read());
    }
  } else {
    _block->isConnected = false;
  }
  return result;
}

/*
   =============
   Populate buffer
   =============
*/

void formatBufferWithBlocks() {
  indexBuffer = 0;
  buffer[indexBuffer++] = 2;
  for (int i = 0; i < QUANTITY_BLOCKS; i++) {
    if (blocks[i]->isConnected) {
      addActiveBlockValues(blocks[i]->id, blocks[i]->quantity, blocks[i]->values);
    } else {
      addInactiveBlockMessage(blocks[i]->id, blocks[i]->quantity);
    }
  }
  buffer[indexBuffer++] = '\n';
}

void addActiveBlockValues(byte blockId, byte arraySize, BlockValue values[]) {
  buffer[indexBuffer++] = blockId;
  for (int i = 0; i < arraySize; i++) {
    buffer[indexBuffer++] = values[i].getValue();
  }
}

void addInactiveBlockMessage(byte blockId, byte quantity) {
  buffer[indexBuffer++] = 0;
  for (int i = 0; i < quantity; i++) {
    buffer[indexBuffer++] = 0;
  }
}

/*
   =============
   Send buffer as consolidated Serial message
   =============
*/

void sendConsolidatedSerialMessage() {
  Serial.write(buffer, BUFFER_SIZE);
}

/*
   =============
   Create and send individual OSC Messages
   =============
*/

// void sendIndividualOSCMessages() {
//   for (int i = 0; i < QUANTITY_BLOCKS; i++) {
//     if (blocks[i]->isConnected) {
//       //      String message_address = "/probatio_m5/";
//       //      String block_address = String(blocks[i]->string_name);
//       //      String composed_address = String(message_address + block_address);
//       //      char char_address[composed_address.length()];
//       //      composed_address.toCharArray(char_address, composed_address.length());
//       OSCMessage oscMsg("/probatio_m5/");
//       udp.beginPacket(destIP, destPort);
//       for (int j = 0; j < blocks[i]->quantity; i++) {
//         oscMsg.add((int)blocks[i]->values[j].getValue());
//       }
//       oscMsg.send(udp);
//       if (isConnected) {
//         udp.endPacket();
//       }
//     }
//   }
// }

/*
   =============
   Send individual MIDI Messages
   =============
*/

boolean sendIndividualMIDIMessages(int threshold) {
  boolean hasAtLeastABlockConnected = false;
  for (int i = 0; i < QUANTITY_BLOCKS; i++) {
    if (blocks[i]->isConnected) {
      hasAtLeastABlockConnected = hasAtLeastABlockConnected | true;
      for (int valueIndex = 0; valueIndex < blocks[i]->quantity; valueIndex++) {
        if (abs(blocks[i]->values[valueIndex].getValue() - blocks[i]->values[valueIndex].getLastValue()) >= threshold) {

          int currentControl = blocks[i]->control + valueIndex;

          /*
          Here, the block value that goes from 0 to 255 
          now is converted from 0 to 127 for MIDI messages
          */
          int currentValue = blocks[i]->values[valueIndex].getValue() / 2;

          int note = 0;
          boolean isNote = false;

          /*
          ===============================
          The following if-else is just 
          to select the values related to buttons
          */

          // The indexes from 10 to 17 are related to buttons
          if (currentControl >= 10 & currentControl <= 17) {
            note = currentControl + 26;
            isNote = true;
          
          // control 38 is the button of the dof4_joystick_1
          } else if (currentControl == 38) {
            // mapping to MIDI Note 44
            note = 44;
            isNote = true;
          
          // control 42 is the button of the dof4_joystick_2
          } else if (currentControl == 42) {
            // mapping to MIDI Note 45
            note = 45;
            isNote = true;
            
          // control 44 is the button of the crank_1
          } else if (currentControl == 44) {
            // mapping to MIDI Note 46
            note = 46;
            isNote = true;
          
          // control 46 is the button of the dial_1
          } else if (currentControl == 46) {
            // mapping to MIDI Note 46
            note = 47;
            isNote = true;
          } else {
            isNote = false;
          }

          /*
          the if-else for selecting the 
          values related to buttons ends here
          ===============================
          */

          /*
          In the following if-else:
          If the value is mapped as a note (in the previous case, buttons),
          when it reaches 127, send a Note On;
          when it reaches 0, send a Note Off.
          */
          if (isNote) {
            if (currentValue == 127) {
              noteOn(0, note, 127);
              MidiUSB.flush();
            } else if (currentValue == 0) {
              noteOff(0, note, 0);
              MidiUSB.flush();
            }
          } else {
            // if the block's value is not a button,
            // use the control number as the MIDI CC
            controlChange(0, currentControl, currentValue);
            MidiUSB.flush();
          }

        }
      }
    }
  }
  return hasAtLeastABlockConnected;
}

/*
   =============
   Create and send consolidated OSC Message
   =============
*/

// void sendConsolidatedOSCMessage() {
//   OSCMessage oscMsg("/probatio_m5");
//   udp.beginPacket(destIP, destPort);

//   for (int i = 0; i < BUFFER_SIZE; i++) {
//     oscMsg.add((int)buffer[i]);
//   }

//   oscMsg.send(udp);
//   if (isConnected) {
//     udp.endPacket();
//   }
// }

/*
   =============
   DEBUG Dump Buffer
   =============
*/

void debugDumpBuffer() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}

/*
   =============
   MIDI related
   =============
*/

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
