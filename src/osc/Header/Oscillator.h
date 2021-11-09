#include "../../Header/Global.h"
#include "../../Header/Buffer.h"

#ifndef PLATFORM_EMBEDDED
    #include <math.h>
    #define TWO_PI (2 * M_PI)
#endif

class Oscillator {
    public:
        Oscillator(float frequency, float phase_offset, Buffer* output);
        ~Oscillator();

        void setFrequency(float frequency);
        void calculatePhaseStep();
        void incrementPhase();

    protected:
        Buffer* output;

        float frequency;
        float phase;
        float phase_step;
        float phase_offset;
};