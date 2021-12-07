//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/Square.h"

Square::Square(float frequency, float phase_offset, Buffer* output) : Oscillator(frequency, phase_offset, output) {

}

Square::~Square() {}

void Square::process() {
  incrementPhase();
  int harmonics = 10;
  output->flush();

  for(int i = 1; i < harmonics; i++) {
    int n = (2 * i) - 1;
    output->writeAddition(((sin(TWO_PI * n * phase) / n) * SAMPLE_MAX_VALUE));
  }
}