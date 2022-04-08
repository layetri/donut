#include "Filter.h"

struct Biquad : Filter {
    Biquad(int delay, Coefficients* input_coefficients, Buffer* input_buffer, Buffer* output_buffer) : Filter(delay, input_coefficients, input_buffer, output_buffer) {};
    ~Biquad();

    void process();
};