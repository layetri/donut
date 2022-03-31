//
// Created by Daniël Kamp on 28/03/2022.
//

#include "Header/Sampler.h"

Sampler::Sampler(Buffer* sample, ParameterPool* params, uint8_t voice_id) : Source(params, voice_id) {
	// Global sample buffer: DO NOT TICK
	this->sample = sample;
	this->start = params->get(p_Sampler_Start, voice_id);
	this->length = params->get(p_Sampler_Length, voice_id);
	this->speed = params->get(p_Sampler_Speed, voice_id);
	this->direction = params->get(p_Sampler_Direction, voice_id);
	position = (uint) this->start->value;
}

Sampler::~Sampler () {}

void Sampler::process() {
	if(position < length->value) {
		position += speed->value;
	} else {
		switch(repeat) {
			case oneshot:
				return;
			case bounce:
				speed->value = -1 * speed->value;
				position += speed->value;
				break;
			case circular:
				position = 0;
				break;
		}
	}
	output->write(sample->getSample(position));
}