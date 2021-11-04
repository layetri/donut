//
// Created by Daniël Kamp on 25/02/2021.
//

#ifndef _SNOWSTORM_LOWPASSFILTER_H
#define _SNOWSTORM_LOWPASSFILTER_H

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

    int16_t calculateSample() override;
    void frequencyHandler() override;

  private:
    // Filter coefficients, butterworth-style
    double b0, b1, b2, a1, a2;
    float frequency;
};


#endif //_SNOWSTORM_LOWPASSFILTER_H
