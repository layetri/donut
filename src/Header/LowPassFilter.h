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
using namespace std;

class LowPassFilter : public Filter {
  public:
    LowPassFilter(Parameter* frequency, Buffer* input, Buffer* output);
    ~LowPassFilter();

    void process();
    void frequencyHandler();
    void frequencyHandler_RBJ();
	void setFrequency(float frequency);
	void setResonance(float res);

	void printCoef();

private:
    // Filter coefficients, butterworth-style
    double b0, b1, b2, a0, a1, a2, q;
    Parameter* frequency;
	float old_f;
};