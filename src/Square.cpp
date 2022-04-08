//
// Created by Daniël Kamp on 09/11/2021.
//

#include <Source/Square.h>

Square::Square(float frequency, ParameterPool* params, uint8_t voice_id) : Oscillator(frequency, params, voice_id) {

}

Square::~Square() {}

void Square::process() {
  incrementPhase();
  int overtones = 10;
  output->flush();

  for(int i = 1; i < overtones; i++) {
    int n = (2 * i) - 1;
    output->writeAddition(((sin(TWO_PI * n * phase) / n) * SAMPLE_MAX_VALUE));
  }
}