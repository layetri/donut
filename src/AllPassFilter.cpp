#include "Header/AllPassFilter.h"

AllPassFilter::AllPassFilter(int delay, Coefficients* input_coefficients, Buffer *input_buffer, Buffer* output_buffer) : Filter(delay, input_coefficients, input_buffer, output_buffer) {
    delay_line = new Buffer(delay);
    delay_line->wipe();
}

AllPassFilter::~AllPassFilter() {
    delete delay_line;
}

void AllPassFilter::process_DF1() {
    fir(n);
    iir(n);
}

void AllPassFilter::process_DF2() {
    sample_t past = delay_line->readBack(8);

    delay_line->write(input->getCurrentSample() + c->a0 * past);
    output->write(c->a0 * delay_line->getCurrentSample() + c->a1 * past);

    delay_line->tick();
}

void AllPassFilter::process_Schroeder() {
    sample_t past = input->readBack(n);
    output->write((-1.0 * c->a0 + past) / (1.0 - c->a0 * past));
}