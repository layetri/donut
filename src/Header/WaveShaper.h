//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_SQUARE_H
#define DONUT_SQUARE_H

#include "Oscillator.h"

class WaveShaper : public Oscillator {
  	public:
		WaveShaper(float, float, Buffer*);
		~WaveShaper();

		void process();
		void setHarmonics(int n);

	private:
		int harmonics;
};


#endif //DONUT_SQUARE_H
