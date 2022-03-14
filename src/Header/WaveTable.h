//
// Created by Daniël Kamp on 13/03/2022.
//

#ifndef DONUT_WAVETABLE_H
#define DONUT_WAVETABLE_H

#include <cmath>
#include <vector>

#include "Global.h"
#include "Buffer.h"
#include "Tables.h"
#include "ParameterStore.h"

#include <iostream>

using namespace std;

class WaveTableOscillator {
	public:
		WaveTableOscillator(Tables* tables, Buffer*, ParameterPool*);
		~WaveTableOscillator();

		void process();
		void setMix(float);
		void tick();
		void pitch(uint8_t);

	private:
		float clip(float, float, float);
		float mtof(uint8_t, float);
		Buffer* square;
		Buffer* sine;
		Buffer* triangle;

		Buffer* output;

		float position;
		int fl_position;
		ParameterPool* parameters;

		sample_t prev_square, prev_sine, prev_triangle;

		float mixer;
		float mix_square, mix_sine, mix_triangle;
		float frequency;
};


#endif //DONUT_WAVETABLE_H
