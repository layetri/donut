//
// Created by Daniël Kamp on 18/03/2022.
//

#ifndef DONUT_AUTOMASTER_H
#define DONUT_AUTOMASTER_H

#include <vector>

#include <Global.h>
#include <DSP/Buffer.h>

#include <Effect/StereoDelay.h>
#include <Effect/Fold.h>

#include <System/Voice.h>
#include <System/ParameterStore.h>

using namespace std;

class AutoMaster {
	public:
		AutoMaster(vector<unique_ptr<Voice>>& voices, ParameterPool* params, Parameter* volume);
		~AutoMaster();
		
		void process();
		void tick();
		void block(size_t block_size);
		float scale(sample_t sample);
		
		float getLeftChannel();
		float getRightChannel();
		
	private:
		vector<unique_ptr<Voice>>& voices;
		Parameter *volume, *delay_amount;
		Buffer output_left, output_right;
		Buffer *master_left, *master_right;
		
//		Fold folder_left, folder_right;
		StereoDelay delay;
	
		uint16_t stereoize;
		uint8_t on_voices;
		long left, right;
		float scaling_sample, mult = 0.0f;
};


#endif //DONUT_AUTOMASTER_H
