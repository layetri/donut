#include "Oscillator.h"

class Saw : Oscillator {
    public:
        Saw(float frequency, ParameterPool* params, uint8_t voice_id);
        ~Saw();

        void process();
};