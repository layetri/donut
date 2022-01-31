#include "Header/ADSR.h"

ADSR2::ADSR2(int attack, int decay, float sustain, int release) {
	this->attack = attack;
	this->decay = decay;
	this->sustain = sustain;
	this->release = release;

	multiplier = 0.0;
	phase = ph_idle;

	regen();
}

ADSR2::~ADSR2() {}

double ADSR2::getMultiplier() {
	return multiplier;
}

void ADSR2::regen() {
	a_step = 1.0 / attack;
	d_step = (1.0 - sustain) / decay;
	r_step = sustain / release;
}

void ADSR2::start() {
	phase = ph_attack;
	counter = 0;
}

void ADSR2::noteOff() {
	phase = ph_release;
}

void ADSR2::step() {
	if(phase != ph_idle) {
		if (phase == ph_attack) {
			multiplier += a_step;
			multiplier = (multiplier > 1.0 ? 1.0 : multiplier);

			if (counter > attack || multiplier == 1.0)
				phase = ph_decay;
		} else if (phase == ph_decay) {
			multiplier -= d_step;
			multiplier = (multiplier < 0.0 ? 0.0 : multiplier);

			if (multiplier == 0.0) {
				phase = ph_idle;
			} else if (counter > (attack + decay)) {
				phase = ph_sustain;
			}
		} else if (phase == ph_sustain) {
			multiplier = sustain;
		} else if (phase == ph_release && multiplier > 0.0) {
			multiplier -= r_step;
			multiplier = (multiplier < 0.0 ? 0.0 : multiplier);
		} else {
			phase = ph_idle;
		}
		counter++;
	}
}

void ADSR2::setAttack(int attack) {
	this->attack = attack;
	regen();
}

void ADSR2::setDecay(int decay) {
	this->decay = decay;
	regen();
}

void ADSR2::setSustain(float sustain) {
	this->sustain = sustain;
	regen();
}

void ADSR2:: setRelease(int release) {
	this->release = release;
	regen();
}