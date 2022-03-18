//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_WAVESHAPER_H
#define DONUT_WAVESHAPER_H

#include "Source.h"

class WaveShaper : public Source {
  	public:
		WaveShaper(ParameterPool*, SourceID, uint8_t);
		~WaveShaper();

		void process() override;
		void tick() override;
		void refresh() override;
		void pitch(uint8_t midi_note) override;
		void fm(float val) override;
		
		void setFrequency(float frequency) override;

	private:
		int harmonics;
		float detune;
		SourceID sid;
};


#endif //DONUT_WAVESHAPER_H
