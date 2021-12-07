//
// Created by Daniël Kamp on 17/11/2021.
//

#include "Header/AddAndDivide.h"

AddAndDivide::AddAndDivide(vector<Buffer*>* inputs, int num_channels, Buffer* output) {
  this->num_channels = num_channels;
  this->inputs = inputs;

  this->output = output;
}

AddAndDivide::~AddAndDivide() {}

void AddAndDivide::process() {
  sample_t val = 0;

  for(auto& b : *inputs) {
    val += (b->getCurrentSample() / num_channels);
  }

  if(multiplier > 0.00) {
	  output->write(val * multiplier);
  }
}

void AddAndDivide::setMultiplier(double mult) {
  this->multiplier = mult;
}