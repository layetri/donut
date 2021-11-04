#pragma once

#include "Global.h"

#if !defined(PLATFORM_DARWIN_X86)
  #include "Arduino.h"

  class Trigger {
    public:
      explicit Trigger(int pin, bool debounce);
      ~Trigger();

      void detect();

      void setStatus(bool status);
      bool getStatus();

      void setDebounce(bool debounce);
    private:
      int pin;
      int timer;
      bool status;
      bool debounce;
  };
#endif