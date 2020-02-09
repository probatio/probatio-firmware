#ifndef Block_h
#define Block_h

#include "Arduino.h"
#include "BlockValue.h"

#define MAX_ARRAY_SIZE 5

class Block
{
  public:
    boolean isConnected;
    byte id;
    byte quantity;
    BlockValue values[MAX_ARRAY_SIZE];
    byte channel;
    byte instrument;
    Block(byte id, byte quantity, byte channel, byte instrument);
  private:
};

#endif
