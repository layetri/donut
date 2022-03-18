#include "Oscillator.h"

class Supersaw : Oscillator {
    public:
        Supersaw(float frequency, ParameterPool* params, uint8_t voice_id);
        ~Supersaw();

        void process();
};