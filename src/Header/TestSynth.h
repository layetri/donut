//
// Single-header test synthesizer for audio development purposes.
// Depends on Buffer.h for I/O.
// Created by Daniël Kamp on 25/06/2021.
//

#ifndef CAVES_TESTSYNTH_H
#define CAVES_TESTSYNTH_H

#include <cmath>
#include "Buffer.h"

class Synth {
  public:
    Synth(float dir, float start, int samplerate, Buffer* output) {
      out_buffer = output;
      this->samplerate = samplerate;
      setFrequency(mtof(steps[0]));

      sample = 0;
      phase = 0.0;
      step = 0;
      cnt = 0;

      max = 500;
      min = 50;
      direction = dir;
      f = start;
    };
    ~Synth() {};

    // Generate a sample and write to output buffer.
    void tick() {
      sample = (int16_t) (sin(phase * M_PI * 2.0) * 32768.0);
      out_buffer->write(sample * gain[step]);

      phase += phase_step;
      if (phase > 1.0) {
        phase -= 1.0;
      }

      if(!sweeping) {
        cnt++;
        if (cnt > samplerate) {
          cnt = 0;
          step++;
          if (step > 5) step = 0;

          setFrequency(mtof(steps[step]));
        }
      } else {
        sweep();
      }
    };

    void setFrequency(float n_frequency) {
      frequency = n_frequency;
      phase_step = frequency / samplerate;
    };

  private:
    float mtof(int note) {
      return pow(2.0, (note - 69.0) / 12.0) * 440.0;
    }

    void sweep() {
      f += direction;

      if(f > max || f < min) {
        direction *= -1;
      }
      setFrequency(f);
    }

    Buffer* out_buffer;
    int16_t sample;

    int samplerate;
    float frequency;
    float phase;
    float phase_step;

    int cnt;
    int step;
    int steps[6] {36, 48, 60, 72, 84, 96};
    float gain[6] {0.5, 0.55, 0.6, 0.65, 0.8, 1.0};

    float f, direction;
    int min, max;
    bool sweeping = true;
};

#endif //CAVES_TESTSYNTH_H
