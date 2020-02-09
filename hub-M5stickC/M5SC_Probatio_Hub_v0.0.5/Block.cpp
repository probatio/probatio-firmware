#include "Arduino.h"
#include "Block.h"

Block::Block(byte _id, byte _quantity, byte _channel, byte _instrument)
{
  id = _id;
  quantity = _quantity;
  channel = _channel;
  instrument = _instrument;
  isConnected = false;
  for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
    values[i] = BlockValue();
  }
}
