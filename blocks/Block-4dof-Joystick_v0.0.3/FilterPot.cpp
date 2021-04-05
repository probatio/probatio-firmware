#include "FilterPot.h"

FilterPot::FilterPot(int bitRange_, float filteringAmount_)
{
  bitRange = bitRange_;
  threshold_ = bitRange_;
  sensorValue = 0;
  filteredValue = 0;
  lastValue_ = 0;
  potSection_ = 0;
  filteringAmount = filteringAmount_;
}

void FilterPot::updateValue(int sensorValue_)
{
  sensorValue = sensorValue_;
  filteredValue = EMA(sensorValue, filteredValue, filteringAmount);
}

bool FilterPot::hasValueChanged() {
  int inputValue_ = filteredValue;
  if (abs(inputValue_ - lastValue_) >= threshold_) {
    lastValue_ = inputValue_;
    return true;
  }
  else return false;
}

int FilterPot::EMA(int inputValue, int filteredValue, float filteringAmount) {
  return ((1 - filteringAmount) * inputValue) + (filteringAmount * filteredValue);
}

int FilterPot::getFilteredValue() {
  return sensorValue / bitRange;
  //return filteredValue / bitRange;
}
