//
// Created by Daniël Kamp on 12/02/2021.
//

#ifndef DONUT_AD_H
#define DONUT_AD_H

#include "Global.h"
#include "MultiFloatBuffer.h"

using namespace std;

class AD {
    public:
        // Takes attack and decay in samples
        AD(int attack, int decay, int voices=NUMBER_OF_VOICES);
        virtual ~AD();

		void start(int voice);
		void generate();
		void setAttack(int attack);
		void setDecay(int decay);
		float getValue(int voice);

	private:
		MultiFloatBuffer* envelope;
		int attack = 0;
		int decay = 0;
		long tick_counter;
};


#endif //DONUT_AD_H
