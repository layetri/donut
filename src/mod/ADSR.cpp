#include "Header/ADSR.h"

ADSR::ADSR(EnvelopeParameters* params) {
    this->params = params;
    mod_buffer = new Buffer(samplerate * 10);
}

ADSR::~ADSR() {
    delete mod_buffer;
}

void ADSR::process() {
    has_started = true;
    if(running) {
        mod_buffer->tick();
    }
}

void ADSR::start() {
    reset();
    running = true;
}

void ADSR::reset() {
    mod_buffer->setPosition(0);
}

void ADSR::refresh() {
    reset();
    for(int i = 0; i < params->total_length(); i++) {
        mod_buffer->write()
    }
}