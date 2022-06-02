#include <DSP/FloatBuffer.h>

FloatBuffer::FloatBuffer(int length) {
    this->length = length;
    data = new float[length];
	position = 0;
	
	for(int i = 0; i < length; i++) {
		data[i] = 0.0;
	}
}

FloatBuffer::~FloatBuffer() {
    delete data;
}

void FloatBuffer::tick() {
    position = (position + 1) * (position < length - 1);
}

void FloatBuffer::write(float sample) {
    data[position] = sample;
}

float FloatBuffer::getSample() {
    return data[position];
}

float FloatBuffer::getSampleAt(uint n) {
	n = (n >= length) * length +
		  (n <= 0) * 0 +
		  (n > 0 && n < length) * n;
	
	return data[n];
}

void FloatBuffer::purge() {
	// Reset all samples in buffer
	for(uint i = 0; i < length; i++) {
		data[i] = 0.0;
	}
	
	// Reset position
	position = 0;
}

void FloatBuffer::setPosition(int position) {
  if(position > 0 && position < length) {
    this->position = position;
  }
}