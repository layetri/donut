#include "Oscillator.h"

class Saw : Oscillator {
    public:
        Saw(float, float, Buffer*);
        ~Saw();

        void process();
};