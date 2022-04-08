#include <Source/Oscillator.h>

Oscillator::Oscillator(float frequency, ParameterPool* params, uint8_t voice_id) : Source(params, voice_id) {
    this->frequency = frequency;
	this->base_frequency = frequency;

    calculatePhaseStep();
}

Oscillator::~Oscillator() {}

void Oscillator::setFrequency(float frequency) {
    this->frequency = frequency;
    calculatePhaseStep();
}

float Oscillator::getFrequency() {
	return this->frequency;
}

void Oscillator::setBaseFrequency(float base_frequency) {
	this->base_frequency = base_frequency;
	setFrequency(mtof(note, base_frequency));
}

void Oscillator::calculatePhaseStep() {
    phase_step = frequency / samplerate;
}

void Oscillator::incrementPhase() {
    phase = (phase + phase_step < 1.0) * (phase + phase_step);
}

void Oscillator::pitch(uint8_t midi_num) {
	note = midi_num;
	setFrequency(mtof(midi_num, base_frequency));
}