//
// Created by Daniël Kamp on 13/03/2022.
//

#include "Header/WaveTable.h"

WaveTableOscillator::WaveTableOscillator(Tables* tables, Buffer* output, ParameterPool* params) {
	square = tables->getSquare();
	sine = tables->getSine();
	triangle = tables->getTriangle();
	parameters = params;

	// Set initial values
	setMix(0.0);
	position = 0;
	frequency = 440.0;
	this->output = output;

	prev_square = 0;
	prev_sine = 0;
	prev_triangle = 0;
}

WaveTableOscillator::~WaveTableOscillator() {}

void WaveTableOscillator::process() {
	sample_t sample = (sample_t) (mix_square * (0.3 * square->getSample(position) + 0.7 * prev_square)) +
		(sample_t) (mix_sine * (0.3 * sine->getSample(position) + 0.7 * prev_sine)) +
		(sample_t) (mix_triangle * (0.3 * triangle->getSample(position) + 0.7 * prev_triangle));

	prev_square = square->getSample(position);
	prev_sine = sine->getSample(position);
	prev_triangle = triangle->getSample(position);

	output->write(sample);
}

void WaveTableOscillator::tick() {
	position = position + (uint) frequency;
	position = (position < samplerate) * position + (position >= samplerate) * position - samplerate;
}

void WaveTableOscillator::setMix(float mix) {
	mixer = mix;

	mix_square = 1.0 - clip(mixer, 0.0, 1.0);
	mix_sine = clip(mixer, 0.0, 1.0) - clip(mixer-1.0, 0.0, 1.0);
	mix_triangle = clip(mixer-1.0, 0.0, 1.0);
}

void WaveTableOscillator::pitch(uint8_t midi_note) {
	frequency = samplerate / mtof(midi_note, 440.0);
	verbose(frequency);
//	frequency = clip(frequency, TABLE_FREQUENCY, samplerate/TABLE_FREQUENCY);
}

float WaveTableOscillator::clip(float value, float bottom, float top) {
	return (value < top && value > bottom) * value + (value > top) * top * (value < bottom) * bottom;
}
float WaveTableOscillator::mtof(uint8_t midi_num, float base_freq) {
	return pow(2.0, (midi_num - 69.0) / 12.0) * base_freq;
}