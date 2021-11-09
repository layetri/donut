#include "Header/FloatBuffer.h"

FloatBuffer::FloatBuffer(int length) {
    this->length = length;
    data = new float[length];
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