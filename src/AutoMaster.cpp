//
// Created by Daniël Kamp on 18/03/2022.
//

#include "Header/AutoMaster.h"

AutoMaster::AutoMaster(vector<Voice*>* voices, Parameter* volume) {
	this->voices = voices;
	this->volume = volume;
	output_left = 0.0;
	output_right = 0.0;
}

AutoMaster::~AutoMaster () {}

void AutoMaster::process () {
	left = 0;
	right = 0;
	on_voices = 0;
	
	for(auto& voice : *voices) {
		voice->process();
		
		left += voice->getSample();
		right += voice->getBuffer()->readBack((samplerate/1000.0) * 5);
		
		on_voices += (voice->isAvailable() != 0);
	}
	
//	if(left != 0) {
//		verbose(left);
//	}

//	on_voices = on_voices > 0 ? on_voices : 1;
	
	output_left = scale(left, on_voices);
	output_right = scale(right, on_voices);
}

void AutoMaster::tick () {
	for(auto& voice : *voices) {
		voice->tick();
	}
}

float AutoMaster::scale(long sample, uint8_t number_of_voices) {
	// Apply conversion and soft clipping on master out
	scaling_sample = (sample * (0.5 + (0.5 / number_of_voices))) / (float) SAMPLE_MAX_VALUE;
	return (scaling_sample >= 1.0) * 0.666666666666667 +
		   (scaling_sample <= -1.0) * -0.666666666666667 +
		   (scaling_sample > -1.0 && scaling_sample < 1.0) * (scaling_sample - (pow(scaling_sample, 3) / 3.0));
}

float AutoMaster::getLeftChannel () {
	return output_left * volume->value;
}

float AutoMaster::getRightChannel () {
	return output_right * volume->value;
}