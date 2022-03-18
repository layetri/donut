//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/Modulator.h"

Modulator::Modulator (ParameterPool* params, uint8_t voice_id) {
	this->parameters = params;
	this->voice_id = voice_id;
}

Modulator::~Modulator () {}

void Modulator::set (float) {}

void Modulator::process () {}

float Modulator::getCurrentValue() {
	return buffer->getSample();
}

FloatBuffer* Modulator::getBuffer() {
	return buffer;
}