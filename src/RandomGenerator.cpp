//
// Created by Daniël Kamp on 19/05/2022.
//

#include <Modulator/RandomGenerator.h>

// If sync > 0: sync to internal clock generator/BPM
RandomGenerator::RandomGenerator(Parameter* rate, Parameter* range, Parameter* slew, Parameter* bpm_sync, ModID modId, string name, uint8_t voice_id) : Modulator(modId, name, voice_id) {
	this->rate = rate;
	this->range = range;
	this->bpm_sync = bpm_sync;
	this->slew = slew;
}

RandomGenerator::~RandomGenerator () {

}

void RandomGenerator::process() {
	// Calculate interval
	interval = (float) samplerate / rate->value;
	counter++;
	
	// Do timer
	if(!bpm_sync->value && counter >= interval) {
		// Generate number
		n = ((float) rand() / (float) RAND_MAX) * range->value;
		counter = 0;
	}
	
	// Apply slew to smooth out values
	n = (1.0f-slew->value) * n + slew->value * n_1;
	n_1 = n;
	
	// Write to output
	buffer->write(n);
}

void RandomGenerator::syncToScheduler() {
	if(bpm_sync->value > 0.0f) {
		n = ((float) rand() / (float) RAND_MAX) * range->value;
	}
}