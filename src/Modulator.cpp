//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/Modulator.h"

Modulator::Modulator (ModID mod_id, string name, uint8_t voice_id) {
	this->mod_id = mod_id;
	this->voice_id = voice_id;
	this->name = name;
	this->buffer = new FloatBuffer(500);
}

Modulator::~Modulator () {}

void Modulator::set (float) {}
void Modulator::start (float) {}
void Modulator::process () {}
void Modulator::refresh () {}
void Modulator::tick () {
	buffer->tick();
}
void Modulator::sync () {}
void Modulator::noteOff () {}

float Modulator::getCurrentValue() {
	return buffer->getSample();
}

FloatBuffer* Modulator::getBuffer() {
	return buffer;
}