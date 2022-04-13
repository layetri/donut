//
// Created by Daniël Kamp on 21/03/2022.
//

#include <System/ModMatrix.h>

ModMatrix::ModMatrix () {}
ModMatrix::~ModMatrix () {}

void ModMatrix::link (Parameter* parameter, Modulator* modulator, uint8_t voice) {
	connections.push_back(new ModDestination {
		modulator, parameter, voice
	});
}

void ModMatrix::process() {
	for(auto& c : connections) {
		c->parameter->value = c->parameter->base_value + (c->modulator->getCurrentValue() * c->amount);
	}
}

vector<ModDestination*>* ModMatrix::get() {
	return &connections;
}

void ModMatrix::store (Modulator* modulator) {
	modulators.push_back(modulator);
}

Modulator *ModMatrix::get (string name, uint8_t voice_id) {
	for(auto& m : modulators) {
		if(m->getName() == name && m->getVoice() == voice_id) {
			return m;
		}
	}
	return modulators[0];
}

bool ModMatrix::clearAll () {
	connections.clear();
	return connections.empty();
}