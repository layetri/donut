//
// Created by Daniël Kamp on 25/02/2021.
//

#ifndef _SNOWSTORM_DELAYLINE_H
#define _SNOWSTORM_DELAYLINE_H

#include "Buffer.h"
#include "Global.h"
#include "ParameterStore.h"

#if defined(PLATFORM_TEENSY_40)
  #include <Arduino.h>
#elif defined(PLATFORM_DARWIN_X86)
  #include <cmath>
  #include <cstdint>
#endif

class DelayLine {
  public:
    DelayLine(Parameter* delayTime, Parameter* feedback, Buffer *input);
    ~DelayLine();

    void process();
    void tick();
	Buffer* getBuffer();

  protected:
    Buffer *x;
    Buffer *y;
	Parameter *feedback, *delayTime;

    int position;
};


#endif //_SNOWSTORM_DELAYLINE_H
