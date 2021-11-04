#include "Header/Trigger.h"

#if !defined(PLATFORM_DARWIN_X86)
  Trigger::Trigger(int pin, bool debounce) {
    pinMode(pin, INPUT);
    this->pin = pin;
    this->debounce = debounce;

    timer = 0;
    status = false;
  }

  Trigger::~Trigger() {}

  void Trigger::detect() {
    if(timer == 0 || !debounce) {
      if (digitalRead(pin) == HIGH) {
        setStatus(true);
        timer = 22100;
      } else {
        setStatus(false);
      }
    } else if(timer > 0) {
      timer--;
    }
  }

  void Trigger::setDebounce(bool debounce) {
    this->debounce = debounce;
  }

  void Trigger::setStatus(bool status) {
    this->status = status;
  }

  bool Trigger::getStatus() {
    return status;
  }
#endif