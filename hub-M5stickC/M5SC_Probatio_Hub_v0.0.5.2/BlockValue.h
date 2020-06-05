#ifndef BlockValue_h
#define BlockValue_h

#include "Arduino.h"

#define FILTER_SHIFT 3

class BlockValue
{
  public:
    boolean flagLockUp;
    boolean flagLockDown;
    byte lastNote;
    BlockValue();
    void setValue(byte value);
    byte getValue();
    byte getLastValue();
    byte getVariation();
    int16_t getLeakyIntegration();
  private:
    byte value;
    byte lastValue;
    int32_t filter_reg;
    int16_t filter_input;
    int16_t filter_output;
};

#endif
