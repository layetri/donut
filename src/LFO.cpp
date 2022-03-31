//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/LFO.h"

LFO::LFO (Parameter* frequency, Parameter* sync_amt, Tables *tables, ModID mod_id, string name, uint8_t voice_id) : Modulator(mod_id, name, voice_id) {
	wave = tables->getSine();
	sample = 0;
	
	this->frequency = frequency;
	this->sync_amt = sync_amt;
	this->mod_id = mod_id;
}

void LFO::sync() {
	// If sync is enabled, reset the position
	position = (sync_amt->value > 0.0) * position;
}

void LFO::beatSync() {}

void LFO::process () {
	sample = (sample_t) (0.3 * wave->getSample(fl_position) + 0.7 * sample);
	buffer->write((float) (sample / SAMPLE_MAX_VALUE));
}

void LFO::refresh() {}

void LFO::tick() {
	position += frequency->value;
//	position = wrap(position);
	position = (position < wave->getSize()) * position + (position >= wave->getSize()) * (position - wave->getSize());
	fl_position = floor(position);
	buffer->tick();
}

float LFO::wrap(float position) {
	if(position < wave->getSize()) {
		return position;
	} else {
		return wrap(position - wave->getSize());
	}
}