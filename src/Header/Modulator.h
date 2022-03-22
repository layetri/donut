//
// Created by Daniël Kamp on 09/11/2021.
// Modulator class creates a wrapper around various modulation objects, so that they can be used interchangeably. This makes it possible to swap modulation sources at runtime.
//

#ifndef DONUT_MODULATOR_H
#define DONUT_MODULATOR_H

#include "Global.h"
#include "ParameterStore.h"
#include "FloatBuffer.h"

using namespace std;

class Modulator {
  public:
    Modulator(ModID mod_id, string name, uint8_t voice_id);
    ~Modulator();
	
	virtual void process();
	virtual void refresh();
	virtual void tick();
	virtual void set(float); // Set a stable value
	
	virtual void sync();
	virtual void start(float);
	virtual void noteOff();
	
	FloatBuffer* getBuffer();
	virtual float getCurrentValue();
	
	ModID getModID() {return mod_id;};
	string getName() {return name;};
	uint8_t getVoice() {return voice_id;};

  protected:
	FloatBuffer* buffer;
	Parameter* sync_amt;
	ModID mod_id;
	string name;
	uint8_t voice_id;
    float value; // Modulators output float values, for easy multiplying (may use value/MAX_SAMPLE_VALUE under the hood)
};


#endif //DONUT_MODULATOR_H
