//
// Created by Daniël Kamp on 09/11/2021.
// Modulator class creates a wrapper around various modulation objects, so that they can be used interchangeably. This makes it possible to swap modulation sources at runtime.
//

#ifndef DONUT_MODULATOR_H
#define DONUT_MODULATOR_H

#include "Global.h"
#include "ParameterStore.h"
#include "FloatBuffer.h"

class Modulator {
  public:
    Modulator(ParameterPool* params, uint8_t voice_id);
    ~Modulator();
	
	void process();
	void refresh();
	void tick();
	void set(float); // Set a stable value
	
	FloatBuffer* getBuffer();
	float getCurrentValue();

  protected:
	ParameterPool* parameters;
	FloatBuffer* buffer;
	
	uint8_t voice_id;
    float value; // Modulators output float values, for easy multiplying (may use value/MAX_SAMPLE_VALUE under the hood)
};


#endif //DONUT_MODULATOR_H
