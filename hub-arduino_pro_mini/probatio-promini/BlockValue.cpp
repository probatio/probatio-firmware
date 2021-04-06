#include "Arduino.h"
#include "BlockValue.h"

BlockValue::BlockValue()
{
  value = 0;
  lastValue = 0;
  flagLockUp = true;
  flagLockDown = false;
  lastNote = 0;
}

void BlockValue::setValue(byte _value) {
  lastValue = value;
  value = _value;
}

byte BlockValue::getValue(){
  return value;
}

byte BlockValue::getLastValue(){
  return lastValue;
}

byte BlockValue::getVariation(){
  return (lastValue - value);
}

int16_t BlockValue::getLeakyIntegration(){
//  filter_input = getVariation();
//  // low pass filter to get rid of noise
//  filter_reg = filter_reg - (filter_reg >> FILTER_SHIFT) + filter_input;
//  filter_output = filter_reg >> FILTER_SHIFT;
//  return filter_output;
}
