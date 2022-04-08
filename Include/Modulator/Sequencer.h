//
// Created by Daniël Kamp on 10/11/2021.
//

#ifndef DONUT_SEQUENCER_H
#define DONUT_SEQUENCER_H

#include <vector>
#include <Modulator/Modulator.h>
#include <DSP/Buffer.h>

using namespace std;

class Sequencer {
	public:
		Sequencer(uint16_t);
		~Sequencer();

		void set(uint16_t, uint8_t);
		uint16_t next();
	protected:
		Buffer* steps;
};


#endif //DONUT_SEQUENCER_H
