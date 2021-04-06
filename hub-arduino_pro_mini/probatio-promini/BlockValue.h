#ifndef BlockValue_h
#define BlockValue_h

//#include "Arduino.h"

#define FILTER_SHIFT 3

class BlockValue
{
  public:
    bool flagLockUp;
    bool flagLockDown;
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
};

#endif
