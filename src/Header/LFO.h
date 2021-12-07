//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_LFO_H
#define DONUT_LFO_H

#include "Modulator.h"
#include "Buffer.h"


class LFO {
  public:
    LFO(Modulator* frequency, float phase_offset, Buffer* output);
    ~LFO();

    void process();
};


#endif //DONUT_LFO_H
