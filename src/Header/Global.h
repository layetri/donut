//
// Created by Daniël Kamp on 30/05/2021.
//

#ifndef DONUT_GLOBAL_H
#define DONUT_GLOBAL_H

#define DEVMODE
// Set platform to Darwin x86 (macOS)
#define PLATFORM_DARWIN_X86

// Make sure that there's only one platform at once
#if !defined(PLATFORM_DARWIN_X86)
  // Set platform to Arduino
  //#define PLATFORM_ARDUINO_ARM

  // Set platform to Teensy 4.0 (IMXRT1062)
  #define PLATFORM_TEENSY40
  #define PLATFORM_EMBEDDED
#else
  #define ENGINE_JACK
  #include <cstdint>
  #include "../util/Header/CommandPool.h"

//  CommandPool command_pool();
#endif

// Add global likely/unlikely directives
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

// Provide a global sample template type
typedef int16_t sample_t;
#define SAMPLE_MAX_VALUE 32768

// Provide a global samplerate value;
extern const int samplerate;

// Project-specific global code goes here!
#define NUM_INPUTS 1

#include <complex>
#include <valarray>
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

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

#endif //DONUT_GLOBAL_H
