//
// Created by Daniël Kamp on 20/03/2022.
// A slimmed down version of Tensions, a physical modeling engine.
//

#ifndef DONUT_TENSIONS_H
#define DONUT_TENSIONS_H

#include <Global.h>
#include <DSP/Buffer.h>
#include <DSP/FilterDelayLine.h>
#include <DSP/LowPassFilter.h>
#include <System/Tables.h>
#include <Source/WaveTable.h>
#include <Source/Source.h>

#include <string>

struct Exciter {
	Tables* tables;
	Buffer* buffer;
	
	void excite(string shape, uint8_t length) {
		for(int i = 0; i < length; i++) {
			if(shape == "square") {
				buffer->writeAhead(tables->getSquare()->getSample(i * (samplerate / length)), i);
			} else if(shape == "pulse") {
				buffer->writeAhead((i % 2 * -1) * SAMPLE_MAX_VALUE, i);
			} else if(shape == "noise") {
				buffer->writeAhead(tables->getNoise()->getSample(i * (samplerate / length)), i);
			}
		}
	}
};

class Tensions : public Source {
public:
	Tensions(Tables* tables, ParameterPool* params, uint8_t voice_id);
	~Tensions();
	
	void process() override;
	void pitch(uint8_t note) override;
	void refresh() override;
	void setDelayTime(uint frequency);
private:
	Buffer *dl_in;
	FilterDelayLine* delayLine;
	Exciter* exciter;
	
	Parameter *ks_feedback, *ks_delaytime, *ks_dampening, *ks_exciter, *transpose;
	
	uint8_t trigger_time, remaining_trigger_time;
};


#endif //DONUT_TENSIONS_H
