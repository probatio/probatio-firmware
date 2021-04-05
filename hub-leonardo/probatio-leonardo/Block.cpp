//#include "Arduino.h"
#include "Block.h"

Block::Block(byte _id, byte _quantity, byte _channel, byte _control, char string_name[])
{
  id = _id;
  quantity = _quantity;
  channel = _channel;
  control = _control;
  isConnected = false;
  for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
    values[i] = BlockValue();
    lastValues[i] = BlockValue();
  }
}
