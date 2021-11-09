// Enforce a sample rate of 44.1KHz
const int samplerate = 44100;

#include "osc/Header/Oscillator.h"

#if defined(PLATFORM_DARWIN_X86)
  #include "util/Header/main_darwin.h"

  int main() {
    Darwin::main();
  }
#elif defined(PLATFORM_TEENSY40)
  #include "util/Header/main_teensy.h"

  Teensy::main();
#endif