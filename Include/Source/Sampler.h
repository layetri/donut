//
// Created by Daniël Kamp on 28/03/2022.
//

#ifndef DONUT_SAMPLER_H
#define DONUT_SAMPLER_H

#include <Global.h>
#include <Source/Source.h>

enum RepeatMode {
	oneshot,
	bounce,
	circular
};

struct SamplerRegion {
	Buffer* sample;
	uint8_t start; // MIDI key where the region starts, inclusive
	uint8_t end; // MIDI key where the region ends, inclusive
};

class Sampler : public Source {
	public:
		Sampler(Buffer*, ParameterPool*, uint8_t);
		~Sampler();
		
		void process() override;
	private:
		Buffer* sample;
		vector<SamplerRegion*> regions;
		Parameter *start, *length, *speed, *direction;
		
		RepeatMode repeat;
		uint position;
};


#endif //DONUT_SAMPLER_H
