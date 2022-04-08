//
// Created by Daniël Kamp on 13/03/2022.
//

#ifndef DONUT_WAVETABLE_H
#define DONUT_WAVETABLE_H

#include <vector>

#include <Global.h>
#include <DSP/Buffer.h>
#include <System/Tables.h>
#include <System/ParameterStore.h>
#include <Source/Source.h>

using namespace std;

class WaveTableOscillator : public Source {
	public:
		WaveTableOscillator(Tables* tables, ParameterPool*, Parameter*, Parameter*, Parameter*, SourceID, uint8_t voice_id);
		~WaveTableOscillator();

		void process() override;
		void refresh() override;
		void tick() override;
		void pitch(uint8_t) override;
		float bind(float);

	private:
		Buffer* square;
		Buffer* sine;
		Buffer* triangle;

		float position;
		int fl_position;
		Parameter *detune, *shape, *transpose;
		SourceID sid;

		sample_t prev_square, prev_sine, prev_triangle;

		float mixer;
		float mix_square, mix_sine, mix_triangle;
		float frequency;
};


#endif //DONUT_WAVETABLE_H
