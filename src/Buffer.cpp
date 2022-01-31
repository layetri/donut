//
// Created by Daniël Kamp on 17/02/2021.
//

#include "Header/Buffer.h"

Buffer::Buffer(int length, std::string name) {
  data = new sample_t [length];
  size = length;
  position = 0;
  full_cycle_flag = false;

  this->name = name;
}

Buffer::~Buffer() {
  delete[] data;
}

int Buffer::getSize() {
  return size;
}

int Buffer::getPosition() {
  return position;
}

std::string Buffer::getName() {
  return name;
}

sample_t Buffer::getSample(int sample_position) {
//  int pos = ((sample_position > size || (sample_position * -1) > size) * sample_position % size) +
//      ((sample_position < size && (sample_position * -1) < size) * sample_position);

  if(sample_position < 0) {
    return this->operator[](size + sample_position);
  } else if(sample_position > size) {
    return this->operator[](sample_position % size);
  } else {
    return this->operator[](sample_position);
  }
}

sample_t Buffer::getCurrentSample() {
  return this->operator[](position);
}

void Buffer::tick() {
  position = (position + 1) * (position < size - 1);
}

void Buffer::write(sample_t sample) {
  data[position] = sample;
}

void Buffer::writeAddition(sample_t sample) {
  data[position] += sample;
}

void Buffer::writeAhead(sample_t sample, int places) {
  data[position + places] = sample;
}

void Buffer::writeAtPosition(sample_t sample, int w_position) {
	data[bounds(w_position)] = sample;
}

void Buffer::setPosition(int position) {
  this->position = position;
}

sample_t Buffer::readAhead(int places) {
  return getSample(position + places);
}

sample_t Buffer::readBack(int places) {
  return getSample(position - places);
}

void Buffer::flush() {
  data[position] = 0;
}

void Buffer::wipe() {
  for(int i = 0; i < size; i++) {
    data[i] = 0;
  }
}

int Buffer::bounds(int value) {
	return (value > 0 && value < size) * value +
		(value < 0) * 0 +
		(value >= size) * (size - 1);
}