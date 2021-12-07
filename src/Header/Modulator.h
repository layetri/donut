//
// Created by Daniël Kamp on 09/11/2021.
// Modulator class creates a wrapper around various modulation objects, so that they can be used interchangeably. This makes it possible to swap modulation sources at runtime.
//

#ifndef DONUT_MODULATOR_H
#define DONUT_MODULATOR_H

#include "Global.h"


class Modulator {
  public:
    Modulator();
    ~Modulator();

    virtual void process();
    void set(float); // Set a stable value
    float getCurrentValue();

  protected:
    float value; // Modulators output float values, for easy multiplying (may use value/MAX_SAMPLE_VALUE under the hood)
};


#endif //DONUT_MODULATOR_H
