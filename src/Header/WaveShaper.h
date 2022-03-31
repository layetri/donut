//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_WAVESHAPER_H
#define DONUT_WAVESHAPER_H

#include "Source.h"

class WaveShaper : public Source {
  	public:
		WaveShaper(ParameterPool*, Parameter*, Parameter*, Parameter*, SourceID, uint8_t);
		~WaveShaper();

		void process() override;
		void pitch(uint8_t midi_note) override;
		void fm(float val, float amount) override;
		
		void setFrequency(float frequency) override;

	private:
		Parameter *detune, *harmonics, *transpose;
		SourceID sid;
};


#endif //DONUT_WAVESHAPER_H
