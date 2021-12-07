#include "Header/PanningInterface.h"
#define TWO_PI (2 * M_PI)

PanningInterface::PanningInterface(vector<Buffer*>* buffers, Buffer* n_output_left, Buffer* n_output_right) {
  output_left = n_output_left;
  output_right = n_output_right;

  // Initialize desired number of input channels
  for(int i = 0; i < NUMBER_OF_VOICES; i++) {
    objects[i] = new SpatialObject(buffers->at(i));
  }
}

PanningInterface::~PanningInterface() {
  for(auto& item : objects) {
    delete item;
  }
}

void PanningInterface::process() {
  int left = 0, right = 0;
  for(auto& channel : objects) {
    auto rtn_val = channel->processOutputSample();
    left += rtn_val[0];
    right += rtn_val[1];
  }

  left = 0.3 * (left / NUM_INPUTS) + 0.7 * output_left->readBack(1);
  right = 0.3 * (right / NUM_INPUTS) + 0.7 * output_right->readBack(1);

  output_left->write(left);
  output_right->write(right);
}

void PanningInterface::tick() {
  output_left->tick();
  output_right->tick();

  output_left->flush();
  output_right->flush();
}
