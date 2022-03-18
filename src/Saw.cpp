#include "Header/Saw.h"

Saw::Saw(float frequency, ParameterPool* params, uint8_t voice_id) : Oscillator(frequency, params, voice_id) {}

Saw::~Saw() {}

void Saw::process() {
    
}