#include "Header/Filter.h"

Filter::Filter(int delay, Coefficients* coefficients, Buffer* input_buffer, Buffer* output_buffer) {
    input = input_buffer;
    output = output_buffer;
    
    this->c = coefficients;
    n = delay;
}

Filter::~Filter() {}

void Filter::fir() {
    sample_t s = input->getCurrentSample() * c->a0 + input->readBack(n) * c->a1;

    output->write(s);
}

void Filter::iir() {
    sample_t s = output->getCurrentSample() * c->b0 - output->readBack(n) * c->b1;
        
    output->write(s);
}

void Filter::tick() {
    output->tick();
}

void Filter::setCoefficients(Coefficients* n_c) {
    this->c = n_c;
}

sample_t Filter::getSample() {
    return output->getCurrentSample();
}