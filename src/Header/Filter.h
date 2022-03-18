#include "Buffer.h"
#include "Global.h"

#pragma once

struct Coefficients {
    Coefficients(float i_a0, float i_a1=1.0, float i_a2=1.0, float i_b0=1.0, float i_b1=1.0, float i_b2=1.0) {
        a0 = i_a0;
        a1 = i_a1;
        a2 = i_a2;
        b0 = i_b0;
        b1 = i_b1;
        b2 = i_b2;
    };

    float a0, a1, a2, b0, b1, b2;
};

struct Filter {
    Filter(int, Coefficients*, Buffer*, Buffer*);
    ~Filter();

    void fir(int delay);
    void iir(int delay);

    void tick();
    void setCoefficients(Coefficients*);
    void bypass();

    sample_t getSample();

    protected:
        Buffer* input;
        Buffer* output;

        Coefficients* c;

        int n;
};
