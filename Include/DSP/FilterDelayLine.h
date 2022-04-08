//
// Created by Daniël Kamp on 25/02/2021.
//

#ifndef _DONUT_FILTERDELAYLINE_H
#define _DONUT_FILTERDELAYLINE_H

#include "../Global.h"
#include "Buffer.h"
#include <System/ParameterStore.h>
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
};


#endif //_SNOWSTORM_DELAYLINE_H
