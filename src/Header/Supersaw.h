#include "Oscillator.h"

class Supersaw : Oscillator {
    public:
        Supersaw(float, float, Buffer*);
        ~Supersaw();

        void process();
};