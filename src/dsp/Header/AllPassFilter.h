#include "../../Header/Buffer.h"
#include "Filter.h"

#pragma once

struct AllPassFilter : public Filter {
    AllPassFilter(int, Coefficients*, Buffer* input_buffer, Buffer* output_buffer);
    ~AllPassFilter();

    void process_DF1();
    void process_DF2();
    void process_Schroeder();

    Buffer* delay_line;
};