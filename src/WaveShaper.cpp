//
// Created by Daniël Kamp on 09/11/2021.
//

#include <Source/WaveShaper.h>

WaveShaper::WaveShaper(Tables* tables, ParameterPool* parameters, Parameter* detune, Parameter* harmonics, Parameter* transpose, SourceID sid, uint8_t voice_id) : Source(parameters, voice_id) {
	this->sid = sid;
	base_frequency = 440.0;
	this->harmonics = harmonics;
	this->detune = detune;
	this->transpose = transpose;
	
	this->sine = tables->getSine();
	
	h = 0;
	old_harmonics = 0.0;
	
	for(int i = 0; i < 16; i++) {
		n[i] = (2 * (i+1)) - 1;
	}
}

WaveShaper::~WaveShaper() {}

void WaveShaper::process() {
	phase_step = (frequency + detune->value) / samplerate;
	phase = (phase + phase_step < 1.0) * (phase + phase_step);
  	output->flush();
  	h = harmonics->value;
	  
  	if(h < 0) {
		for (int i = 0; i < h * -1; i++) {
//			output->writeAddition(sine->getSample(floor((float) (n[i] * samplerate * phase)) / n[i]) / 2.0f);
			output->writeAddition(((sin(TWO_PI * n[i] * phase) / n[i]) * SAMPLE_MAX_VALUE));
		}
	} else if(h > 0) {
	  	for(int i = 1; i < h; i++) {
//			output->writeAddition(sine->getSample(floor((float) (i * samplerate * phase) / i) / 2.0f));
			output->writeAddition(((sin(TWO_PI * i * phase) / i) * SAMPLE_MAX_VALUE));
		}
	} else {
//		output->writeAddition(sine->getSample(floor((float) (samplerate * phase)) / 2.0f));
		output->write(sin(TWO_PI * phase) * SAMPLE_MAX_VALUE);
  	}
}

void WaveShaper::pitch (uint8_t midi_note) {
	frequency = mtof((uint8_t) (midi_note + transpose->value), BASE_TUNING);
	base_frequency = frequency + detune->value;
	phase_step = base_frequency / samplerate;
}

void WaveShaper::setFrequency (float frequency) {
	this->frequency = frequency;
	phase_step = (frequency + detune->value) / samplerate;
}

/**
 * Applies Frequency Modulation to the source
 * @note uses base_frequency, which is set to 440.0 by default
 * @param val The value of the modulation source at the given tick [0.0, 1.0]
 */
void WaveShaper::fm(float val, float amount) {
	setFrequency((base_frequency * 2 * abs(val) * amount) + base_frequency);
}