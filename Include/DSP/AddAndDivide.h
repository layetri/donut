//
// Created by Daniël Kamp on 17/11/2021.
// A simple Add-and-Divide mixer to merge multiple channels into one.
//

#ifndef DONUT_ADDANDDIVIDE_H
#define DONUT_ADDANDDIVIDE_H

#include "Buffer.h"
#include <vector>

#include <DSP/HighPassFilter.h>
#include <System/ParameterStore.h>

using namespace std;

class AddAndDivide {
  public:
    AddAndDivide(vector<Buffer*>* inputs, ParameterPool* params, uint8_t voice_id, Buffer* output);
    ~AddAndDivide();

    void process();
	void tick();

  private:
    Buffer *filter_queue, *output;
	HighPassFilter* output_filter;
    vector<Buffer*>* inputs;
	ParameterPool* parameters;
	Parameter *ws1, *ws2, *wt1, *wt2, *ks, *smp, *part, *master;
};


#endif //DONUT_ADDANDDIVIDE_H
