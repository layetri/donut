//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_WAVESHAPER_H
#define DONUT_WAVESHAPER_H

#include <Source/Source.h>
#include <System/Tables.h>

class WaveShaper : public Source {
  	public:
		WaveShaper(Tables* tables, ParameterPool*, Parameter*, Parameter*, Parameter*, SourceID, uint8_t);
		~WaveShaper();

		void process() override;
		void pitch(uint8_t midi_note) override;
		void fm(float val, float amount) override;
		
		void setFrequency(float frequency) override;

	private:
		Buffer *sine;
		Parameter *detune, *harmonics, *transpose;
		SourceID sid;
		float h, old_harmonics;
		int n[16];
};


#endif //DONUT_WAVESHAPER_H
