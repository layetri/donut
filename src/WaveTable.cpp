//
// Created by Daniël Kamp on 13/03/2022.
//

#include "Header/WaveTable.h"

WaveTableOscillator::WaveTableOscillator(Tables* tables, ParameterPool* params, Parameter* detune, Parameter* shape, Parameter* transpose, SourceID sid, uint8_t voice_id) : Source(params, voice_id) {
	square = tables->getSquare();
	sine = tables->getSine();
	triangle = tables->getTriangle();
	this->detune = detune;
	this->shape = shape;
	this->sid = sid;
	this->transpose = transpose;

	// Set initial values
	position = 0.0;
	fl_position = 0;
	frequency = 440.0;
	base_frequency = 430.0;

	prev_square = 0;
	prev_sine = 0;
	prev_triangle = 0;

	refresh();
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
	position += frequency;
	position = (position < square->getSize()) * position + (position >= square->getSize()) * (position - square->getSize());
//	position = bind(position);
	fl_position = floor(position);
	
//	output->tick();
}

void WaveTableOscillator::refresh() {
	mixer = 0.8 * mixer + 0.2 * shape->value;

	mix_square = 1.0 - clip(mixer, 0.0, 1.0);
	mix_sine = clip(mixer, 0.0, 1.0) - clip(mixer-1.0, 0.0, 1.0);
	mix_triangle = clip(mixer-1.0, 0.0, 1.0);
	
	this->base_frequency = detune->value;
}

void WaveTableOscillator::pitch(uint8_t midi_note) {
	frequency = (samplerate / TABLE_FREQUENCY) / mtof(127-(midi_note + 2 + (uint8_t) transpose->value), base_frequency);
}

float WaveTableOscillator::bind(float value) {
	if(value >= square->getSize()) {
		return bind(value - square->getSize());
	} else {
		return value;
	}
}