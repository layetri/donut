//
// Created by Daniël Kamp on 17/02/2021.
//

#ifndef SNOWSTORM_BUFFER_H
#define SNOWSTORM_BUFFER_H

#include <string>
#include "Global.h"

#if defined(PLATFORM_TEENSY_40)
  #include <Arduino.h>
#elif defined(PLATFORM_DARWIN_X86)
  #include <cmath>
  #include <cstdint>
#endif

class Buffer {
public:
    Buffer(uint length, std::string name="Generic Buffer");
    ~Buffer();

    void write(sample_t sample);
    void writeAhead(sample_t sample, int places);
    void writeAddition(sample_t sample);
	void writeAtPosition(sample_t sample, int position);

    void tick();
    void flush();
    void wipe();
    void setPosition(int position);
	void setMultiplier(float multiplier);

    uint getPosition();
    uint getSize();
    std::string getName();

    sample_t getSample(int sample_position);
    sample_t getCurrentSample();
    sample_t getCurrentSampleMultiplied();
    sample_t readAhead(int places);
    sample_t readBack(int places);

    sample_t& operator[] (int index) {
      return data[index];
    }

private:
	uint bounds(int value);
    sample_t *data;
    uint size;
    uint position;
    std::string name;
	float multiplier;

    bool full_cycle_flag;
};

#endif //SNOWSTORM_BUFFER_H
