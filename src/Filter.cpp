#include "Header/Filter.h"

Filter::Filter(int delay, Coefficients* coefficients, Buffer* input_buffer, Buffer* output_buffer) {
    input = input_buffer;
    output = output_buffer;
    
    this->c = coefficients;
    n = delay;
}

Filter::~Filter() {}

void Filter::fir(int delay) {
    sample_t s = input->getCurrentSample() * c->a0 + input->readBack(delay) * c->a1;

    output->write(s);
}

void Filter::iir(int delay) {
    sample_t s = output->getCurrentSample() * c->b0 - output->readBack(delay) * c->b1;
        
    output->write(s);
}

void Filter::tick() {
    output->tick();
}

void Filter::setCoefficients(Coefficients* n_c) {
    this->c = n_c;
}

void Filter::bypass() {
	output->write(input->getCurrentSample());
}

sample_t Filter::getSample() {
    return output->getCurrentSample();
}