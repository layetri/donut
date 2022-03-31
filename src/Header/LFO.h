//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_LFO_H
#define DONUT_LFO_H

#include "Modulator.h"
#include "Buffer.h"
#include "Tables.h"
#include "ParameterStore.h"

using namespace std;

class LFO : public Modulator {
  public:
    LFO(Parameter* frequency, Parameter* sync_amt, Tables* tables, ModID mod_id, string name, uint8_t voice_id);
    ~LFO();
	
	void tick() override;
	void refresh() override;
    void process() override;
	void sync() override;
	void beatSync();
	float wrap(float);

  private:
	Buffer* wave;
	Parameter *frequency;
	
	float position;
	uint16_t fl_position;
	sample_t sample;
};


#endif //DONUT_LFO_H
