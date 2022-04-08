#include <Source/Sine.h>

Sine::Sine(float frequency, ParameterPool* params, uint8_t voice_id) : Oscillator(frequency, params, voice_id) {

}

Sine::~Sine() {

}

void Sine::process() {
    incrementPhase();
    output->write(sin(TWO_PI * phase) * SAMPLE_MAX_VALUE);
}