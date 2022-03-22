//
// Created by Daniël Kamp on 25/02/2021.
//

#include "Header/DelayLine.h"

DelayLine::DelayLine(Parameter* delayTime, Parameter* feedback, Buffer *input) {
  this->delayTime = delayTime;
  this->feedback = feedback;
  position = 0;

  x = input;
  y = new Buffer(input->getSize(), "DelayLine");
}

DelayLine::~DelayLine() {
  delete y;
}

// Increment the buffer position
void DelayLine::tick() {
  if(position < x->getSize()) {
    position++;
  } else {
    position -= x->getSize();
  }
}

void DelayLine::process() {
  sample_t sample;
  // Run the delay line
  sample = 0.3 * x->getSample((int) (position - delayTime->value)) +
		0.7 * x->getSample((int) (position - delayTime->value) - 1) +
		((y->getSample((int) (position - delayTime->value) - 1) +
		y->getSample((int) (position - delayTime->value))) * 0.5 * feedback->value);

  // Store the sample in the output buffer
  y->write(sample);
}

Buffer* DelayLine::getBuffer() {
	return y;
}