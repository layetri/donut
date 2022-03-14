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
	position = 0.0;
	fl_position = 0;
	frequency = 440.0;
	this->output = output;

	prev_square = 0;
	prev_sine = 0;
	prev_triangle = 0;
}

WaveTableOscillator::~WaveTableOscillator() {}

void WaveTableOscillator::process() {
	sample_t sample = (sample_t) (mix_square * (0.3 * square->getSample(fl_position) + 0.7 * prev_square)) +
		(sample_t) (mix_sine * (0.3 * sine->getSample(fl_position) + 0.7 * prev_sine)) +
		(sample_t) (mix_triangle * (0.3 * triangle->getSample(fl_position) + 0.7 * prev_triangle));

	prev_square = square->getSample(fl_position);
	prev_sine = sine->getSample(fl_position);
	prev_triangle = triangle->getSample(fl_position);

	output->write(sample);
}

void WaveTableOscillator::tick() {
	position = position + frequency;
	position = (ceil(position) < square->getSize()) * position + (ceil(position) >= square->getSize()) * (position - square->getSize());
	fl_position = floor(position);
}

void WaveTableOscillator::setMix(float mix) {
	mixer = 0.8 * mixer + 0.2 * mix;

	mix_square = 1.0 - clip(mixer, 0.0, 1.0);
	mix_sine = clip(mixer, 0.0, 1.0) - clip(mixer-1.0, 0.0, 1.0);
	mix_triangle = clip(mixer-1.0, 0.0, 1.0);
}

void WaveTableOscillator::pitch(uint8_t midi_note) {
	frequency = (samplerate / TABLE_FREQUENCY) / mtof(127-(midi_note+2), 430.0);
}

float WaveTableOscillator::clip(float value, float bottom, float top) {
	return (value < top && value > bottom) * value + (value >= top) * top * (value <= bottom) * bottom;
}
float WaveTableOscillator::mtof(uint8_t midi_num, float base_freq) {
	return pow(2.0, (midi_num - 69.0) / 12.0) * base_freq;
}