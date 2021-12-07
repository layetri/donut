#include "Header/Sine.h"

Sine::Sine(float frequency, float phase_offset, Buffer* output) : Oscillator(frequency, phase_offset, output) {

}

Sine::~Sine() {

}

void Sine::process() {
    incrementPhase();
    output->write(sin(TWO_PI * phase) * SAMPLE_MAX_VALUE);
}