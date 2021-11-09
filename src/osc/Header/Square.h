//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_SQUARE_H
#define DONUT_SQUARE_H

#include "Oscillator.h"

class Square : Oscillator {
  public:
    Square(float, float, Buffer*);
    ~Square();

    void process();
};


#endif //DONUT_SQUARE_H
