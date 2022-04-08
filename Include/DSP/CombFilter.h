#include "Filter.h"

struct CombFilter : public Filter {
    CombFilter(int delay, Coefficients* coefficients, Buffer* input, Buffer* output);
    ~CombFilter();

    void process();
};