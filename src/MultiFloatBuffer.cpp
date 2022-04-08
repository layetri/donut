//
// Created by Daniël Kamp on 06/12/2021.
//

#include <DSP/MultiFloatBuffer.h>

MultiFloatBuffer::MultiFloatBuffer(int length, int buffers) : FloatBuffer(length) {
	for(int i = 0; i < buffers; i++) {
		readheads.push_back(0);
	}
}

MultiFloatBuffer::~MultiFloatBuffer() {}

float MultiFloatBuffer::getMultiSample(int buffer) {
	return data[readheads[buffer]];
}

void MultiFloatBuffer::multiTick(int buffer) {
	int o = readheads[buffer];

	readheads[buffer] = (o + 1) * (o + 1 < length);
}

void MultiFloatBuffer::setMultiPosition(int buffer, int position) {
	if(position > 0 && position < length) {
		readheads[buffer] = position;
	}
}

int MultiFloatBuffer::getMultiPosition(int buffer) {
	return readheads[buffer];
}