//
// Created by Daniël Kamp on 30/05/2021.
//

#ifndef KARPLUSSTRONG_GLOBAL_H
#define KARPLUSSTRONG_GLOBAL_H

#define DEVMODE
// Set platform to Darwin x86 (macOS)
#define PLATFORM_DARWIN_X86

// Add global likely/unlikely directives
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

// Provide a global sample template type
typedef int16_t sample_t;

// Make sure that there's only one platform at once
#if !defined(PLATFORM_DARWIN_X86)
  // Set platform to Arduino
  //#define PLATFORM_ARDUINO_ARM

  // Set platform to Teensy 4.0 (IMXRT1062)
  #define PLATFORM_TEENSY40
#endif

// Global logging function
#ifdef DEVMODE
  #if defined(PLATFORM_DARWIN_X86)
    #include <string>
    #include <iostream>
  #else
    #include <Arduino.h>
  #endif

  template <typename T>
  void verbose(const T& message) {
    #if defined(PLATFORM_DARWIN_X86)
      std::cout << message << std::endl;
    #else
      Serial.println(message);
    #endif
  }
#endif

#endif //KARPLUSSTRONG_GLOBAL_H
