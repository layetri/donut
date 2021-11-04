//
// Created by Daniël Kamp on 25/02/2021.
//

#include "Header/LowPassFilter.h"

LowPassFilter::LowPassFilter(float frequency, int samplerate, Buffer *input, Buffer *output) : Filter(1, c, input, output) {
  this->frequency = frequency;
  frequencyHandler();
}

LowPassFilter::~LowPassFilter() {}

void LowPassFilter::frequencyHandler() {
  const double ita = 1.0/ tan(M_PI*ff);
  const double q = sqrt(2.0);

  // Set coefficients
  b0 = 1.0 / (1.0 + q*ita + ita*ita);
  b1 = 2*b0;
  b2 = b0;
  a1 = 2.0 * (ita*ita - 1.0) * b0;
  a2 = -(1.0 - q*ita + ita*ita) * b0;

  Coefficients t(1.0, a1, a2, b0, b1, b2);
  setCoefficients(&t);
}
