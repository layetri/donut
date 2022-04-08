//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_SQUARE_H
#define DONUT_SQUARE_H

#include <Source/Oscillator.h>

class Square : public Oscillator {
  public:
    Square(float frequency, ParameterPool* params, uint8_t voice_id);
    ~Square();

    void process();
};


#endif //DONUT_SQUARE_H
