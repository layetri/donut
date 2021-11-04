#include "Header/Knob.h"

#if !defined(PLATFORM_DARWIN_X86)
  Knob::Knob(int pin) {
    pinMode(pin, INPUT);
    this->pin = pin;
  }

  Knob::~Knob() {}

  float Knob::getValue() {
    value = analogRead(pin) / 1023.0;
    return value;
  }
#endif