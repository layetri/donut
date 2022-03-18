//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_LFO_H
#define DONUT_LFO_H

#include "Modulator.h"
#include "Buffer.h"
#include "Tables.h"
#include "ParameterStore.h"

class LFO : public Modulator {
  public:
    LFO(float frequency, Tables* tables, ParameterPool* params, uint8_t voice_id);
    ~LFO();
	
	void tick();
	void refresh();
    void process();

  private:
	Buffer* wave;
	float position, frequency;
	
	uint16_t fl_position;
};


#endif //DONUT_LFO_H
