#pragma once

#include "Global.h"

#if !defined(PLATFORM_DARWIN_X86)
  #include "Arduino.h"

  class Knob {
    public:
      explicit Knob(int pin);
      ~Knob();

      float getValue();

    private:
      int pin;
      float value;
  };
#endif