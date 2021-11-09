//
// Created by Daniël Kamp on 25/02/2021.
//
#pragma once

#include "Global.h"

#if !defined(PLATFORM_DARWIN_X86)
  #include <Arduino.h>
#elif defined(PLATFORM_DARWIN_X86)
  #include <cmath>
  #include <cstdint>
#endif

#include "Filter.h"

class LowPassFilter : public Filter {
  public:
    LowPassFilter(float frequency, int samplerate, Buffer* input, Buffer* output);
    ~LowPassFilter();

    int16_t calculateSample();
    void frequencyHandler();

  private:
    // Filter coefficients, butterworth-style
    double b0, b1, b2, a1, a2;
    float frequency;
};