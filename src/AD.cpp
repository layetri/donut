//
// Created by Daniël Kamp on 12/02/2021.
//

#include "Header/AD.h"

AD::AD(int attack, int decay, int voices) {
	envelope = new MultiFloatBuffer(MAX_ENVELOPE_LENGTH, voices);

	setAttack(attack);
	setDecay(decay);
}

AD::~AD() {

}

void AD::start(int voice) {
	envelope->setMultiPosition(voice, 0);
}

void AD::generate() {
	double att_step = 1.0 / attack;
	double dec_step = 1.0 / decay;
//	rel_step = sus_level / sustain;

	for(int i = 0; i < attack; i++) {
		envelope->write(i * att_step);
		envelope->tick();
	}
	for(int i = 0; i < decay; i++) {
		envelope->write(1.0 - (i * dec_step));
		envelope->tick();
	}
	envelope->setPosition(0);
}

float AD::getValue(int voice) {
	if(envelope->getMultiPosition(voice) < attack + decay) {
		float out = envelope->getMultiSample(voice);
		envelope->multiTick(voice);
		return out;
	}
	return 0.0;
}

void AD::setAttack(int attack) {
	this->attack = (attack < MAX_ENVELOPE_LENGTH / 2) ? attack : MAX_ENVELOPE_LENGTH;
	generate();
}

void AD::setDecay(int decay) {
	this->decay = (decay < MAX_ENVELOPE_LENGTH / 2) ? decay : MAX_ENVELOPE_LENGTH;
	generate();
}