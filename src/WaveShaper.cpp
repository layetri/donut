//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/WaveShaper.h"

WaveShaper::WaveShaper(float frequency, float phase_offset, Buffer* output) : Oscillator(frequency, phase_offset, output) {
	harmonics = 2;
}

WaveShaper::~WaveShaper() {}

void WaveShaper::process() {
  	incrementPhase();
  	output->flush();
  	if(harmonics < 0) {
		int h = harmonics * -1;
		for (int i = 0; i < h; i++) {
			int n = (2 * (i+1)) - 1;
			output->writeAddition(((sin(TWO_PI * n * phase) / n) * SAMPLE_MAX_VALUE));
		}
	} else if(harmonics > 0) {
		  for(int i = 0; i < harmonics; i++) {
			  output->writeAddition(((sin(TWO_PI * i * phase) / i) * SAMPLE_MAX_VALUE));
		  }
	} else {
		  output->write(sin(TWO_PI * phase) * SAMPLE_MAX_VALUE);
	  }
}

void WaveShaper::setHarmonics(int n) {
	this->harmonics = n;
}