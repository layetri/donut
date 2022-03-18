#include "Oscillator.h"

class Sine : public Oscillator {
    public:
        Sine(float frequency, ParameterPool* params, uint8_t voice_id);
        ~Sine();

        void process();
};