//
// Created by Daniël Kamp on 18/03/2022.
//

#include <Effect/AutoMaster.h>

AutoMaster::AutoMaster(vector<Voice*>* voices, ParameterPool* params, Parameter* volume) {
	this->voices = voices;
	this->volume = volume;
	output_left = new Buffer(samplerate/2);
	output_right = new Buffer(samplerate/2);
	
	this->delay = new StereoDelay(params, output_left, output_right);
	this->delay_amount = params->get(p_FX_Delay_Amount, 0);
	
	master_left = delay->getLeftChannel();
	master_right = delay->getRightChannel();
	stereoize = (samplerate/1000.0) * 5;
}

AutoMaster::~AutoMaster () {}

void AutoMaster::process () {
	left = 0;
	right = 0;
	on_voices = 0;
	
	for(auto& voice : *voices) {
		voice->process();

		left += voice->getSample();
		right += voice->getBuffer()->readBack(stereoize);

		on_voices += !(voice->isAvailable());
	}
	
	on_voices = on_voices + (on_voices == 0);
	
	output_left->write((sample_t) (left * (sqrt(1.0 / on_voices))));
	auto tmp = (sample_t) (left * (sqrt(1.0 / on_voices)));
	output_right->write((sample_t) (right * (sqrt(1.0 / on_voices))));
//	if(tmp != 0) {
//		verbose(tmp);
//	}
//	verbose(output_left->getCurrentSample());
	
	delay->process();
}

void AutoMaster::tick () {
	for(auto& voice : *voices) {
		voice->tick();
	}
	
	output_left->tick();
	output_right->tick();
	
	master_left->tick();
	master_right->tick();
}

float AutoMaster::scale(sample_t sample) {
	// Apply conversion and soft clipping on master out
	scaling_sample = (float) sample / (float) SAMPLE_MAX_VALUE;
	return (scaling_sample >= 1.0) * 0.666666666666667 +
		   (scaling_sample <= -1.0) * -0.666666666666667 +
		   (scaling_sample > -1.0 && scaling_sample < 1.0) * (scaling_sample - (pow(scaling_sample, 3) / 3.0));
}

float AutoMaster::getLeftChannel () {
	return scale((output_left->getCurrentSample() + (master_left->getCurrentSample()) * delay_amount->value)) * volume->value;
//	return scale(master_left->getCurrentSample()) * volume->value;
}

float AutoMaster::getRightChannel () {
	return scale((output_right->getCurrentSample() + (master_right->getCurrentSample()) * delay_amount->value)) * volume->value;
//	return scale(master_right->getCurrentSample()) * volume->value;
}