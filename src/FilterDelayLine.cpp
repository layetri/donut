//
// Created by Daniël Kamp on 25/02/2021.
//

#include "Header/FilterDelayLine.h"
#include <iostream>

FilterDelayLine::FilterDelayLine (Parameter *delayTime, Parameter *feedback, Parameter *cutoff, Buffer *input,
								  Buffer *output) {
	this->delayTime = delayTime;
	this->feedback = feedback;
	this->cutoff = cutoff;
	position = 0;
	
	x = input;
	y = new Buffer(input->getSize(), "FilterDelayLine");
	z = output;
	
	filter = new LowPassFilter(6000.0, y, z);
}

FilterDelayLine::~FilterDelayLine () {
	delete y;
}

// Increment the buffer position
void FilterDelayLine::tick () {
	y->tick();
}

void FilterDelayLine::process () {
	// Run the delay line
	int dt = (int) (samplerate / delayTime->value);
	y->write(x->readBack(dt) +
		((y->readBack(dt + 1) +
		y->readBack(dt)) * 0.5 * feedback->value));
	
	// Store the sample in the output buffer
	filter->process();
	z->write((y->readBack((int) delayTime->value) + z->getCurrentSample()) * 0.5);
}