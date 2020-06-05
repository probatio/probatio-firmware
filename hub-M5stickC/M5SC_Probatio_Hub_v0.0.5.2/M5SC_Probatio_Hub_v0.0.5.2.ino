// Direct OSC Version: sends 44 integers for each sensor
// Needs hardcoding of IP, port, and SSID

#include <M5StickC.h>
#include "Wire.h"
#include "Block.h"
#include "BlockValue.h"
#include "auxData.h"
#include <probatio_defs.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "esp_wifi.h"

#define SOFTAP 0
#define D0 1

WiFiUDP udp;

char ssid[] = "YOUR_WIFI_SSID"; //  your network SSID (name)
char pass[] = "YOUR_WIFI_PASSWORD";

bool isConnected = false;

IPAddress destIP(192, 168, 1, 116); //
const unsigned int destPort = 7000;

int cycle = 0;

byte buffer[BUFFER_SIZE];
int indexBuffer = 0;

boolean loopCycleControl = true;
int loopCycleTime = 100;

int quantBlocksConnected = 0;

Block dual_buttons_1(BLOCK_DUAL_BUTTONS_1, SIZE_BLOCK_DUAL_BUTTONS_1, 0, 0, "dual_buttons_1");
Block obsf_24(BLOCK_OBSF_24, SIZE_BLOCK_OBSF_24, 0, 0, "obsf_24");
Block obsf_30(BLOCK_OBSF_30, SIZE_BLOCK_OBSF_30, 0, 0, "obsf_30");
Block cherries_1(BLOCK_CHERRIES_1, SIZE_BLOCK_CHERRIES_1, 0, 0, "cherries_1");
Block dual_pots_1(BLOCK_DUAL_POTS_1, SIZE_BLOCK_DUAL_POTS_1, 0, 0, "dual_pots_1");
Block tap_1(BLOCK_TAP_1, SIZE_BLOCK_TAP_1, 0, 0, "tap_1");
Block tap_2(BLOCK_TAP_2, SIZE_BLOCK_TAP_2, 0, 0, "tap_2");
Block dual_pistons_1(BLOCK_DUAL_PISTONS_1, SIZE_BLOCK_DUAL_PISTONS_1, 0, 0, "dual_pistons_1");
Block joystick_1(BLOCK_JOYSTICK_1, SIZE_BLOCK_JOYSTICK_1, 0, 0, "joystick_1");
Block four_pistons_1(BLOCK_FOUR_PISTONS_1, SIZE_BLOCK_FOUR_PISTONS_1, 0, 0, "four_pistons_1");
Block four_thumbs_1(BLOCK_FOUR_THUMBS_1, SIZE_BLOCK_FOUR_THUMBS_1, 0, 0, "four_thumbs_1");
Block dof4_joystick_1(BLOCK_DOF4_JOYSTICK_1, SIZE_BLOCK_DOF4_JOYSTICK_1, 0, 0, "dof4_joystick_1");
Block dof4_joystick_2(BLOCK_DOF4_JOYSTICK_2, SIZE_BLOCK_DOF4_JOYSTICK_2, 0, 0, "dof4_joystick_2");
Block crank_1(BLOCK_CRANK_1, SIZE_BLOCK_CRANK_1, 0, 0, "crank_1");
Block dial_1(BLOCK_DIAL_1, SIZE_BLOCK_DIAL_1, 0, 0, "dial_1");
Block debug(BLOCK_DEBUG, SIZE_BLOCK_DEBUG, 0, 0, "debug");
Block* blocks[QUANTITY_BLOCKS] = {&dual_buttons_1, &obsf_24, &obsf_30, &cherries_1, &dual_pots_1, &tap_1, &tap_2, &dual_pistons_1, &joystick_1, &four_pistons_1, &four_thumbs_1, &dof4_joystick_1, &dof4_joystick_2, &crank_1, &dial_1, &debug};

void setup() {

  /*
      ==============
      M5 Setup
      ==============
  */

  M5.begin();
  M5.Lcd.fillScreen(BLUE);
  M5.Lcd.setTextSize(5);
  dacWrite(25, 0);
  Wire.begin(0, 26);
  Serial.begin(115200);

  /*
     ==============
     Wifi Setup
     ==============
  */

  esp_wifi_set_ps(WIFI_PS_NONE);
  Serial.print(F("Connecting to WiFi..."));
  if (!SOFTAP) {
    WiFi.begin(ssid, pass);
  }
  else {
    WiFi.softAP(ssid, pass);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP is: ");
    Serial.println(IP);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F(""));
  Serial.println(F("WiFi connected"));
  isConnected = true;

  Serial.println();
  Serial.print(F("IP address is "));
  if (SOFTAP) Serial.println(WiFi.softAPIP());
  else Serial.println(WiFi.localIP());
}

void loop() {

  long t0 = millis();

  //M5.Lcd.fillScreen(YELLOW);

  requestI2C();
  formatBufferWithBlocks();

  //sendConsolidatedSerialMessage();
  //debugDumpBuffer();
  //sendIndividualOSCMessages();
  sendConsolidatedOSCMessage();

  long t1 = millis();
  long deltaT = (t1 - t0);
  if (loopCycleControl) {
    if (deltaT < loopCycleTime) {
      //delay(loopCycleTime - deltaT);
    }
  }

  M5.Lcd.setCursor(20, 20);
  M5.Lcd.print(quantBlocksConnected);
}

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

void sendIndividualOSCMessages() {
  for (int i = 0; i < QUANTITY_BLOCKS; i++) {
    if (blocks[i]->isConnected) {
      //      String message_address = "/probatio_m5/";
      //      String block_address = String(blocks[i]->string_name);
      //      String composed_address = String(message_address + block_address);
      //      char char_address[composed_address.length()];
      //      composed_address.toCharArray(char_address, composed_address.length());
      OSCMessage oscMsg("/probatio_m5/");
      udp.beginPacket(destIP, destPort);
      for (int j = 0; j < blocks[i]->quantity; i++) {
        oscMsg.add((int)blocks[i]->values[j].getValue());
      }
      oscMsg.send(udp);
      if (isConnected) {
        udp.endPacket();
      }
    }
  }
}

/*
   =============
   Create and send consolidated OSC Message
   =============
*/

void sendConsolidatedOSCMessage() {
  OSCMessage oscMsg("/probatio_m5");
  udp.beginPacket(destIP, destPort);

  for (int i = 0; i < BUFFER_SIZE; i++) {
    oscMsg.add((int)buffer[i]);
  }

  oscMsg.send(udp);
  if (isConnected) {
    udp.endPacket();
  }
}

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
