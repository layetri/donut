//
// Created by Daniël Kamp on 25/02/2021.
//

#ifndef _SNOWSTORM_DELAYLINE_H
#define _SNOWSTORM_DELAYLINE_H

#include <cmath>
#include <cstdint>

#include <Global.h>
#include <DSP/Buffer.h>
#include <System/ParameterStore.h>

class DelayLine {
  public:
    DelayLine(Parameter* delayTime, Parameter* feedback, Buffer *input);
    ~DelayLine();

    void process();
	Buffer* getBuffer();

  protected:
    Buffer *x;
    Buffer *y;
	Parameter *feedback, *delayTime;
};


#endif //_SNOWSTORM_DELAYLINE_H
