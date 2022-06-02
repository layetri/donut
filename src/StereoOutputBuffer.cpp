//
// Created by Daniel Kamp on 2-6-22.
//

#include <DSP/StereoOutputBuffer.h>

StereoOutputBuffer::StereoOutputBuffer() : left_channel(samplerate/2), right_channel(samplerate/2) {}

StereoOutputBuffer::~StereoOutputBuffer() {}

// Flag to see if the buffer is ready to be written to
bool StereoOutputBuffer::isReadyToWrite() {
	return _ready_to_write;
}

// Flag to see if the buffer is ready to be read from
bool StereoOutputBuffer::isReadyToRead() {
	return _ready_to_read;
}

// Flag to see if the buffer is done reading
bool StereoOutputBuffer::isDoneReading () {
	return hasReachedBlockEnd(samples_read);
}

// Flag to see if the buffer is full
bool StereoOutputBuffer::hasReachedBlockEnd(uint pos) const {
	return pos >= block_size;
}

// Reset the buffer after it's been processed by JACK
void StereoOutputBuffer::recycle() {
	samples_written = 0;
	left_channel.purge();
	right_channel.purge();
	_ready_to_write = true;
}

// Write a pair of floats to the current buffer position
void StereoOutputBuffer::writeSamplePair(float left, float right) {
	left_channel.write(left);
	right_channel.write(right);
	samples_written++;
}

void StereoOutputBuffer::tick() {
	if(hasReachedBlockEnd(samples_written)) {
		_ready_to_write = false;
		_ready_to_read = true;
	} else {
		left_channel.tick();
		right_channel.tick();
	}
}

void StereoOutputBuffer::tickReadHead() {
	if(hasReachedBlockEnd(samples_read)) {
		_ready_to_read = false;
		recycle();
	} else {
		samples_read++;
	}
}

void StereoOutputBuffer::setBlockSize(uint block_size) {
	this->block_size = block_size;
}

float StereoOutputBuffer::getLeftSample() {
	if(!_ready_to_write)
		return left_channel.getSampleAt(samples_read);
	
//	verbose("WARNING: Reading from buffer while it is locked is not permitted.");
	return 0.0;
}

float StereoOutputBuffer::getRightSample() {
	if(!_ready_to_write)
		return right_channel.getSampleAt(samples_read);
	
//	verbose("WARNING: Reading from buffer while it is locked is not permitted.");
	return 0.0;
}