//
// Created by Daniël Kamp on 13/03/2022.
//

#ifndef DONUT_WAVETABLE_H
#define DONUT_WAVETABLE_H

#include <vector>

#include "Global.h"
#include "Buffer.h"
#include "Tables.h"
#include "ParameterStore.h"
#include "Source.h"

using namespace std;

class WaveTableOscillator : public Source {
	public:
		WaveTableOscillator(Tables* tables, ParameterPool*, SourceID, uint8_t voice_id);
		~WaveTableOscillator();

		void process() override;
		void refresh() override;
		void tick() override;
		void pitch(uint8_t) override;

	private:
		Buffer* square;
		Buffer* sine;
		Buffer* triangle;

		float position;
		int fl_position;
		ParameterPool* parameters;
		SourceID sid;

		sample_t prev_square, prev_sine, prev_triangle;

		float mixer;
		float mix_square, mix_sine, mix_triangle;
		float frequency;
};


#endif //DONUT_WAVETABLE_H
