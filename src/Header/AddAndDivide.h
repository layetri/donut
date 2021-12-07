//
// Created by Daniël Kamp on 17/11/2021.
// A simple Add-and-Divide mixer to merge multiple channels into one.
//

#ifndef DONUT_ADDANDDIVIDE_H
#define DONUT_ADDANDDIVIDE_H

#include "Buffer.h"
#include <vector>

using namespace std;

class AddAndDivide {
  public:
    AddAndDivide(vector<Buffer*>* inputs, int num_channels, Buffer* output);
    ~AddAndDivide();

    void process();
    void setMultiplier(double mult);

  private:
    Buffer* output;
    vector<Buffer*>* inputs;
    int num_channels;
    double multiplier = 0.0;
};


#endif //DONUT_ADDANDDIVIDE_H
