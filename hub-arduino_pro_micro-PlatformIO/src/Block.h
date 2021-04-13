#ifndef Block_h
#define Block_h

#include "Arduino.h"
#include "BlockValue.h"

#define MAX_ARRAY_SIZE 5

class Block
{
  public:
    bool isConnected;
    byte id;
    byte quantity;
    BlockValue values[MAX_ARRAY_SIZE];
    BlockValue lastValues[MAX_ARRAY_SIZE];
    byte channel;
    byte control;
    char string_name[];
    Block(byte id, byte quantity, byte channel, byte control, char string_name[]);
    
  private:
};

#endif
