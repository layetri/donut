//
// Created by Daniël Kamp on 09/04/2022.
//

#include <DSP/HighPassFilter.h>

Coefficients coef_hi = {1.0};

HighPassFilter::HighPassFilter (Parameter* frequency, Buffer* input, Buffer* output) : Filter(1, &coef_hi, input, output) {
	this->frequency = frequency;
	this->old_f = 0.0;
	this->q = 0.9;
	handleFrequency();
}
HighPassFilter::~HighPassFilter () {}

void HighPassFilter::process() {
	if(frequency->value != old_f) {
		handleFrequency();
		old_f = frequency->value;
	}
	
	sample_t s = input->getCurrentSample() * a0 +
				 input->readBack(1) * a1 +
				 input->readBack(2) * a2 -
				 output->readBack(1) * b1 -
				 output->readBack(2) * b2;
	
	output->write(s);
}

// Coefficients calculation: © Dimitris Tassopoulos 2016-2020
void HighPassFilter::handleFrequency () {
	const double w = 2.0 * M_PI * frequency->value / samplerate;
	const double d = 1.0 / q;
	const double b = 0.5*(1.0 - (d / 2)*sin(w)) / (1.0 + (d / 2.0)*sin(w));
	const double g = (0.5 + b)*cos(w);
	a0 = (0.5 + b + g) / 2.0;
	a1 = -(0.5 + b + g);
	a2 = a0;
	b1 = -2.0 * g;
	b2 = 2.0 * b;
}
