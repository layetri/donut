#include <DSP/CombFilter.h>

CombFilter::CombFilter(int delay, Coefficients* coefficients, Buffer* input, Buffer* output) : Filter(delay, coefficients, input, output) {}

CombFilter::~CombFilter() {}

void CombFilter::process() {
    iir(n);
}