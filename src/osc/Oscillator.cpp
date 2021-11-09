#include "Header/Oscillator.h"

Oscillator::Oscillator(float frequency, float phase_offset, Buffer* output) {
    this->output = output;
    this->frequency = frequency;
    this->phase_offset = phase_offset;

    calculatePhaseStep();
}

Oscillator::~Oscillator() {}

void Oscillator::setFrequency(float frequency) {
    this->frequency = frequency;
    calculatePhaseStep();
}

void Oscillator::calculatePhaseStep() {
    phase_step = frequency / samplerate;
}

void Oscillator::incrementPhase() {
    phase = (phase + phase_step < 1.0) * (phase + phase_step);
}