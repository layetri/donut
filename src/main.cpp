#include "Header/Global.h"
// Enforce a sample rate of 44.1KHz
unsigned int samplerate = 44100;
#include "Header/main_darwin.h"

int main() {
	program();
}

#if defined(PLATFORM_DARWIN_X86)

#elif defined(PLATFORM_TEENSY40)
  #include "util/Header/main_teensy.h"

  Teensy::main();
#endif