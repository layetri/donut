#include <Modulator/ADSR.h>

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

void ADSR2::regen() {
	a_step = 1.0 / (float) attack->value;
	d_step = (1.0 - sustain->value) / (float) decay->value;
	r_step = sustain->value / (float) release->value;
}

void ADSR2::start(float velocity) {
	phase = ph_attack;
	this->velocity = velocity;
	position = 0;
	multiplier = 0;
	regen();
}

void ADSR2::noteOff() {
	phase = ph_release;
}

void ADSR2::process() {
	if(phase != ph_idle) {
		if(!_is_processing) {
			regen();
			_is_processing = true;
		}
		switch(phase) {
			case ph_attack:
				multiplier += a_step;
//				multiplier = (multiplier > 1.0 ? 1.0 : multiplier);
				
				if (position > attack->value || multiplier >= 1.0) {
					phase = ph_decay;
					multiplier = 1.0;
				}
				break;
			case ph_decay:
				multiplier -= d_step;
				
				if (multiplier <= 0.0) {
					phase = ph_idle;
					multiplier = 0.0;
				} else if (position > (attack->value + decay->value)) {
					phase = ph_sustain;
				}
				break;
			case ph_sustain:
				multiplier = sustain->value;
				break;
			case ph_release:
				multiplier -= r_step;
//				multiplier = (multiplier < 0.0 ? 0.0 : multiplier);
				if (multiplier <= 0.0) {
					phase = ph_idle;
					_is_processing = false;
					multiplier = 0.0;
				}
				break;
			default:
				break;
		}
		
		position++;
	}
	buffer->write(multiplier * velocity);
}