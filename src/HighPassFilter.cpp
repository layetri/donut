//
// Created by Daniël Kamp on 09/04/2022.
//

#include <DSP/HighPassFilter.h>

Coefficients coef_hi = {1.0};

HighPassFilter::HighPassFilter (Parameter* frequency, Buffer* input, Buffer* output) : Filter(1, &coef_hi, input, output) {

}
HighPassFilter::~HighPassFilter () {}

void HighPassFilter::process() {

}
