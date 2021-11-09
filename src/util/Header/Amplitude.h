#include "../../Header/Buffer.h"
#include "../../Header/Global.h"
#include "Biquad.h"

#include <vector>
#include <cmath>
#pragma once
// #define MULTICHANNEL

class Amplitude {
    public:
        Amplitude(Buffer*, Buffer*);
        ~Amplitude();

        std::vector<int>* DetectOnsets();
        void Hilbert();

    private:
        void initializeBiquads();
        
        Buffer* input;
        Buffer* output;

        Coefficients *c1, *c2;
        Biquad *bq1, *bq2;
        Buffer *link1, *analysis_bus;
        
        #ifdef MULTICHANNEL
            Coefficients *c3, *c4;
            Biquad *bq3, *bq4;
            Buffer *link2, *link3, *link4;
        #endif
};