#include <DSP/Biquad.h>

void Biquad::process() {
    sample_t s = (c->a0 * input->getCurrentSample()) +
        (c->a1 * input->readBack(n)) +
        (c->a2 * input->readBack(2*n)) +
        (c->b1 * output->readBack(n)) -
        (c->b2 * output->readBack(2*n));

    output->write(s);
}