#include "Oscillator.h"

class Sine : Oscillator {
    public:
        Sine(float frequency, float phase_offset, Buffer* output);
        ~Sine();

        void process();
};