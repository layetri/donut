//
// Created by Daniël Kamp on 25/02/2021.
//

#include <DSP/DelayLine.h>

DelayLine::DelayLine(Parameter* delayTime, Parameter* feedback, Buffer *input) {
  this->delayTime = delayTime;
  this->feedback = feedback;
  x = input;
  y = new Buffer(input->getSize());
}

DelayLine::~DelayLine() {
  delete y;
}

void DelayLine::process() {
  sample_t sample;
  // Run the delay line
  sample = 0.3 * x->readBack((int) delayTime->value) +
		0.7 * x->readBack((int) (delayTime->value + 1)) +
		((y->readBack((int) (delayTime->value + 1)) +
		y->readBack((int) delayTime->value)) * 0.5 * feedback->value);

  // Store the sample in the output buffer
  y->write(sample);
}

Buffer* DelayLine::getBuffer() {
	return y;
}