//
// Created by Daniël Kamp on 14/03/2022.
//

#ifndef DONUT_SOURCE_H
#define DONUT_SOURCE_H

#include <Global.h>
#include <System/ParameterStore.h>
#include <DSP/Buffer.h>

#include <cmath>
#include <unordered_map>
#include <string>

#define TWO_PI (2 * M_PI)

using namespace std;

class Source {
	public:
		Source(ParameterPool* parameters, uint8_t voice_id);
		~Source();

		virtual void process();
		virtual void tick();
		virtual void refresh();
		virtual void pitch(uint8_t midi_note);
		virtual void setFrequency(float frequency);
		virtual void fm(float val, float amount);
		virtual void block(size_t block_size);

		Buffer* getBuffer();
		static float mtof(uint8_t midi_note, float base_frequency=440.0);
	protected:
		float clip(float, float, float);
		
		Buffer* output;
		ParameterPool* parameters;
		uint8_t voice_id;
		float frequency, base_frequency, old_frequency;
		float phase, phase_step, phase_offset;
};


#endif //DONUT_SOURCE_H
