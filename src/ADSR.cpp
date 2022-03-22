#include "Header/ADSR.h"

ADSR2::ADSR2(Parameter* attack, Parameter* decay, Parameter* sustain, Parameter* release, ModID mod_id, string name, uint8_t voice_id) : Modulator(mod_id, name, voice_id) {
	this->attack = attack;
	this->decay = decay;
	this->sustain = sustain;
	this->release = release;

	multiplier = 0.0;
	velocity = 1.0;
	phase = ph_idle;

	regen();
}

ADSR2::~ADSR2() {}

//float ADSR2::getCurrentValue () {
//	return multiplier * velocity;
//}

void ADSR2::regen() {
	a_step = 1.0 / attack->value;
	d_step = (1.0 - sustain->value) / decay->value;
	r_step = sustain->value / release->value;
}

void ADSR2::start(float velocity) {
	phase = ph_attack;
	this->velocity = velocity;
	position = 0;
}

void ADSR2::noteOff() {
	phase = ph_release;
}

void ADSR2::process() {
	if(phase != ph_idle) {
		// TODO: see if this works (putting todo here to easily find it again, if things break)
		regen();
		if (phase == ph_attack) {
			multiplier += a_step;
			multiplier = (multiplier > 1.0 ? 1.0 : multiplier);

			if (position > attack->value || multiplier == 1.0)
				phase = ph_decay;
		} else if (phase == ph_decay) {
			multiplier -= d_step;
			multiplier = (multiplier < 0.0 ? 0.0 : multiplier);

			if (multiplier == 0.0) {
				phase = ph_idle;
			} else if (position > (attack->value + decay->value)) {
				phase = ph_sustain;
			}
		} else if (phase == ph_sustain) {
			multiplier = sustain->value;
		} else if (phase == ph_release && multiplier > 0.0) {
			multiplier -= r_step;
			multiplier = (multiplier < 0.0 ? 0.0 : multiplier);
		} else {
			phase = ph_idle;
		}
		position++;
	}
	buffer->write(multiplier * velocity);
}