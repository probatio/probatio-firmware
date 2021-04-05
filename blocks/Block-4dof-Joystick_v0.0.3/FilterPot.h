#ifndef FilterPot_h
#define FilterPot_h

#include "Arduino.h"

#define TEN_BIT_READING 1 //this is the lossless option, using all the 10 bits of the Arduino ADC
#define NINE_BIT_READING 2
#define EIGHT_BIT_READING 4
#define SEVEN_BIT_READING 8 //the standard MIDI values are 7 bits long

class FilterPot
{
  public:
    int bitRange;
    int sensorValue;
    int filteredValue;
    //int getSectionizedValue(int nSections);
    float filteringAmount;
    bool hasValueChanged();
    void updateValue(int sensorValue_);
    int getFilteredValue();
    //bool changedValue(int inputValue_, int threshold_);
    FilterPot(int bitRange_, float filteringAmount_);
  private:
    int lastValue_;
    int potSection_;
    int threshold_;
    int EMA(int inputValue, int filteredValue, float filteringAmount);
};

#endif
