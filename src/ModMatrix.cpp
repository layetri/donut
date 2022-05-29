//
// Created by Daniël Kamp on 21/03/2022.
//

#include <System/ModMatrix.h>

ModMatrix::ModMatrix (ParameterPool* parameters) {
	this->parameters = parameters;
	
	dictionary.push_back(ModDict{m_LFO1, "lfo1"});
	dictionary.push_back(ModDict{m_LFO2, "lfo2"});
	dictionary.push_back(ModDict{m_ADSR1, "adsr1"});
	dictionary.push_back(ModDict{m_ADSR2, "adsr2"});
	dictionary.push_back(ModDict{m_RND1, "rnd1"});
	dictionary.push_back(ModDict{m_RND2, "rnd2"});
}
ModMatrix::~ModMatrix () {}

void ModMatrix::link (Parameter* parameter, Modulator* modulator, uint8_t voice, float amount) {
	connections.push_back(new ModDestination {
		modulator, parameter, voice, amount
	});
}

void ModMatrix::process() {
	for(auto& c : connections) {
		c->parameter->value = c->parameter->base_value + (c->modulator->getCurrentValue() * c->amount);
	}
}

vector<Modulator*>* ModMatrix::getMods() {
	return &modulators;
}

vector<ModDict>* ModMatrix::getDict() {
	return &dictionary;
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

Modulator *ModMatrix::get (ModID mid, uint8_t voice_id) {
	for(auto& m : modulators) {
		if(m->getModID() == mid && m->getVoice() == voice_id) {
			return m;
		}
	}
	return modulators[0];
}

ModDestination* ModMatrix::get(Modulator* source, Parameter* destination, uint8_t voice_id) {
	for(auto& c : connections) {
		if(c->modulator->getName() == source->getName() && c->parameter->pid == destination->pid && c->voice_id == voice_id) {
			return c;
		}
	}
	return nullptr;
}

bool ModMatrix::clearAll () {
	connections.clear();
	return connections.empty();
}

void ModMatrix::setOrCreate(ModID source, ParameterID destination, float amount) {
	for(int i = 0; i < NUMBER_OF_VOICES; i++) {
		auto dst = parameters->get(destination, i);
		auto src = get(source, i);
		auto link = get(src, dst, i);
		if(link != nullptr) {
			link->amount = amount;
		}
	}
}