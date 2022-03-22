//
// Created by Daniël Kamp on 18/03/2022.
//

#ifndef DONUT_AUTOMASTER_H
#define DONUT_AUTOMASTER_H

#include <vector>
#include "Global.h"
#include "Buffer.h"
#include "Voice.h"
#include "ParameterStore.h"

using namespace std;

class AutoMaster {
	public:
		AutoMaster(vector<Voice*>* voices, Parameter* volume);
		~AutoMaster();
		
		void process();
		void tick();
		float scale(long sample, uint8_t number_of_voices);
		
		float getLeftChannel();
		float getRightChannel();
		
	private:
		vector<Voice*>* voices;
		Parameter* volume;
		float output_left, output_right;
		
		uint8_t on_voices;
		long left, right;
		float scaling_sample;
};


#endif //DONUT_AUTOMASTER_H
