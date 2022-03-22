//
// Created by Daniël Kamp on 25/02/2021.
//

#ifndef _SNOWSTORM_DELAYLINE_H
#define _SNOWSTORM_DELAYLINE_H

#include "Buffer.h"
#include "Global.h"
#include "ParameterStore.h"
#include "LowPassFilter.h"

#if defined(PLATFORM_TEENSY_40)
  #include <Arduino.h>
#elif defined(PLATFORM_DARWIN_X86)
  #include <cmath>
  #include <cstdint>
#endif

class FilterDelayLine {
  public:
    FilterDelayLine(Parameter* delayTime, Parameter* feedback, Parameter* cutoff, Buffer *input, Buffer* output);
    ~FilterDelayLine();

    void process();
    void tick();
  protected:
    Buffer *x, *y, *z;
	Parameter *feedback, *delayTime, *cutoff;
	LowPassFilter* filter;

    int position;
};


#endif //_SNOWSTORM_DELAYLINE_H
