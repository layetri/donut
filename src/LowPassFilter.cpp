//
// Created by Daniël Kamp on 25/02/2021.
//

#include "Header/LowPassFilter.h"

Coefficients coef = {1.0};
extern unsigned int samplerate;

LowPassFilter::LowPassFilter(float frequency, Buffer *input, Buffer *output) : Filter(1, &coef, input, output) {
  this->frequency = frequency;
  frequencyHandler();
}

LowPassFilter::~LowPassFilter() {}

void LowPassFilter::frequencyHandler() {
  const double ita = 1.0/ tan(M_PI*(frequency / samplerate));
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

void LowPassFilter::frequencyHandler_RBJ() {
	const double w0 = 2 * M_PI * frequency / samplerate;
	const double cw0 = cos(w0);
	const double sw0 = sin(w0);
	const double alpha = sw0 / (2 * q);

	a0 = 1.0f + alpha;
	a1 = (-2.0f * cw0) / a0;
	a2 = (1.0f - alpha) / a0;
	b2 = ((1.0f - cw0) / 2.0f) / a0;
	b0 = ((1.0f - cw0) / 2.0f) / a0;
	b1 = (1.0f - cw0) / a0;
}

void LowPassFilter::setFrequency(float frequency) {
	this->frequency = frequency;
	frequencyHandler_RBJ();
}

void LowPassFilter::process() {
//	output->flush();
	sample_t s = input->getCurrentSample() * b0 +
		input->readBack(1) * b1 +
		input->readBack(2) * b2 -
		output->readBack(1) * a1 -
		output->readBack(2) * a2;

	output->write(s);
}

void LowPassFilter::printCoef() {
	verbose(to_string(b0) + " " + to_string(b1) + " " + to_string(b2) + " " + to_string(a1) + " " + to_string(a2));
}

void LowPassFilter::setResonance(float res) {
	this->q = res;
	frequencyHandler_RBJ();
}
