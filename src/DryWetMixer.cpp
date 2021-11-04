//
// Created by Daniël Kamp on 04/03/2021.
//

#include "Header/DryWetMixer.h"

DryWetMixer::DryWetMixer(float ratio) {
  this->ratio = ratio;
}

DryWetMixer::~DryWetMixer() {}

void DryWetMixer::update() {
//  audio_block_t* block_left, block_right, output_left, output_right;
//  block_left = receiveReadOnly(0);
//  block_right = receiveReadOnly(1);
//
//  for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
//    auto tmp = ratio * input1->getCurrentSample() + (1.0 - ratio) * input2->getCurrentSample());
//    output_left->data[i] = tmp;
//    output_right->data[i] = tmp;
//  }
//
//  transmit(block_left, 0);
//  transmit(block_right, 1);
//
//  release(block_left, block_right);+
}

void DryWetMixer::setRatio(float r) {
  if(r > 1) {
    ratio = 1.0;
  } else if(r < 0) {
    ratio = 0.0;
  } else {
    ratio = r;
  }
}