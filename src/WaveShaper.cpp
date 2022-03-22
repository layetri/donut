//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/WaveShaper.h"

WaveShaper::WaveShaper(ParameterPool* parameters, SourceID sid, uint8_t voice_id) : Source(parameters, voice_id) {
	this->sid = sid;
	base_frequency = 440.0;
	harmonics = 2;
	detune = 0.0;
}

WaveShaper::~WaveShaper() {}

void WaveShaper::process() {
	phase = (phase + phase_step < 1.0) * (phase + phase_step);
	
  	output->flush();
  	if(harmonics < 0) {
		for (int i = 0; i < harmonics * -1; i++) {
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

void WaveShaper::pitch (uint8_t midi_note) {
	frequency = mtof(midi_note, 440.0 + detune);
	phase_step = frequency / samplerate;
	base_frequency = frequency;
}

void WaveShaper::tick () {

}

void WaveShaper::setFrequency (float frequency) {
	this->frequency = frequency;
	phase_step = frequency / samplerate;
}

void WaveShaper::refresh () {
	this->harmonics = parameters->get(p_WS_Harmonics, voice_id)->value;
	
	if(sid == s_WS2) {
		this->detune = parameters->get(p_WS_Detune, voice_id)->value;
	}
}

/**
 * Applies Frequency Modulation to the source
 * @note uses base_frequency, which is set to 440.0 by default
 * @param val The value of the modulation source at the given tick [0.0, 1.0]
 */
void WaveShaper::fm(float val, float amount) {
	setFrequency((base_frequency * 2 * abs(val) * amount) + base_frequency);
}