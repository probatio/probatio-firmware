// Direct OSC Version: sends 44 integers for each sensor
// Needs hardcoding of IP, port, and SSID

#include <M5StickC.h>
#include "Wire.h"
#include "Block.h"
#include "BlockValue.h"
#include "auxData.h"
#include "probatio_defs.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "esp_wifi.h"

#define SOFTAP 0
#define D0 1


WiFiUDP udp;

char ssid[] = "tstick_network"; //  your network SSID (name)
char pass[] = "mappings";

bool isConnected = false;

IPAddress destIP(192, 168, 1, 118); //
const unsigned int destPort = 7000;

int cycle = 0;

byte buffer[BUFFER_SIZE];
int indexBuffer = 0;

boolean loopCycleControl = true;
int loopCycleTime = 100;

Block bellows(BLOCK_BELLOWS, SIZE_BLOCK_BELLOWS, 0, 11);
Block breath(BLOCK_BREATH, SIZE_BLOCK_BREATH, 1, 13);
Block crank(BLOCK_CRANK, SIZE_BLOCK_CRANK, 2, 1);
Block debug(BLOCK_DEBUG, SIZE_BLOCK_DEBUG, 3, 80);
Block joystick(BLOCK_JOYSTICK, SIZE_BLOCK_JOYSTICK, 4, 1);
Block onetap(BLOCK_ONETAP, SIZE_BLOCK_ONETAP, 5, 9);
Block twopots(BLOCK_TWOPOTS, SIZE_BLOCK_TWOPOTS, 6, 12);
Block restouch(BLOCK_RESTOUCH, SIZE_BLOCK_RESTOUCH, 0, 50);
Block threetaps(BLOCK_THREETAPS, SIZE_BLOCK_THREETAPS, 8, 80);
Block turntable(BLOCK_TURNTABLE, SIZE_BLOCK_TURNTABLE, 8, 2);
Block twobuttons1(BLOCK_TWOBUTTONS1, SIZE_BLOCK_TWOBUTTONS1, 10, 1);
Block twobuttons2(BLOCK_TWOBUTTONS2, SIZE_BLOCK_TWOBUTTONS2, 10, 1);
Block twobuttons3(BLOCK_TWOBUTTONS3, SIZE_BLOCK_TWOBUTTONS3, 10, 1);
Block twobuttons4(BLOCK_TWOBUTTONS4, SIZE_BLOCK_TWOBUTTONS4, 10, 1);

//Block* blocks[QUANTITY_BLOCKS] = {&fourButtons, &restouch, &bellows, &crank, &turntable, &debug};
Block* blocks[QUANTITY_BLOCKS] = {&bellows, &breath, &crank, &debug, &joystick, &onetap, &twopots, &restouch,
                                  &threetaps, &turntable, &twobuttons1, &twobuttons2, &twobuttons3, &twobuttons4
                                 };

void setup() {
  M5.begin();
  esp_wifi_set_ps(WIFI_PS_NONE);
  M5.Lcd.fillScreen(YELLOW);
  M5.Lcd.setTextSize(2);
  dacWrite(25, 0);
  Wire.begin(0, 26);
  Serial.begin(115200);

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

  OSCMessage oscMsg("/probatio_m5");
  udp.beginPacket(destIP, destPort);

  M5.Lcd.fillScreen(YELLOW);
  requestI2C();
  formatBufferWithBlocks();
 
  for (int i=0; i<BUFFER_SIZE; i++) {
    oscMsg.add((int)buffer[i]);
    Serial.print((int)buffer[i]);
    Serial.print(" ");
  }
  Serial.println();

  oscMsg.send(udp);
  
  if (isConnected) {
    udp.endPacket();
    //Serial.println("!!!!!!!!!!!!!!!!!!!");
  }
  //DEBUG
  //  for (int i=0; i<BUFFER_SIZE; i++) {
  //    Serial.print(buffer[i], HEX);
  //  }
  //  Serial.println();

  //Serial.write(buffer, BUFFER_SIZE);

  indexBuffer = 0;
  long t1 = millis();
  long deltaT = (t1 - t0);
  //Serial.println(deltaT);
  if (loopCycleControl) {
    if (deltaT < loopCycleTime) {
      delay(loopCycleTime - deltaT);
    }
  }

  //  for(int i = 0; i < QUANTITY_BLOCKS; i++){
  //    if (blocks[i]->isConnected) {
  //      M5.Lcd.fillRect(20, 10*i+10, 5, 5, GREEN);
  //    } else {
  //      M5.Lcd.fillRect(20, 10*i+10, 5, 5, RED);
  //    }
  //  }
}

void requestI2C() {
  for (int i = 0; i < QUANTITY_BLOCKS; i++) {
    requestFromDevice(blocks[i]);
  }
}

void formatBufferWithBlocks() {
  buffer[indexBuffer++] = 2;
  for (int i = 0; i < QUANTITY_BLOCKS; i++) {
    if (blocks[i]->isConnected) {
      addActiveBlockValues(blocks[i]->id, blocks[i]->quantity, blocks[i]->values);
      blocks[i]->isConnected = false;
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

void requestFromDevice(Block * _block) {
  Wire.requestFrom((uint8_t)_block->id, (uint8_t)_block->quantity, (uint8_t)true);
  while (Wire.available())
  {
    //    digitalWrite(i2cPin, HIGH);
    for (int i = 0; i < _block->quantity; i++) {
      _block->values[i].setValue(Wire.read());
    }
    _block->isConnected = true;
  }
  //wdt_reset();
  //digitalWrite(i2cPin, LOW);
}
