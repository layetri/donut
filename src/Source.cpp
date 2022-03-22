//
// Created by Daniël Kamp on 14/03/2022.
//

#include "Header/Source.h"

Source::Source(ParameterPool *parameters, uint8_t voice_id) {
	this->output = new Buffer(500);
	this->parameters = parameters;
	this->voice_id = voice_id;
}

Source::~Source() {}

void Source::process() {}
void Source::refresh() {}
void Source::fm(float val, float amount) {}

void Source::pitch(uint8_t midi_note) {
	frequency = mtof(midi_note);
}
void Source::setFrequency (float frequency) {
	this->frequency = frequency;
}

void Source::tick() {
	output->tick();
}

Buffer* Source::getBuffer() {
	return this->output;
}

float Source::mtof(uint8_t midi_note, float base_frequency) {
	return pow(2.0, (midi_note - 69.0) / 12.0) * base_frequency;
}

float Source::clip(float value, float bottom=0.0, float top=1.0) {
	return (value < top && value > bottom) * value +
		(value >= top) * top +
		(value <= bottom) * bottom;
}