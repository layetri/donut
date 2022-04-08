//
// Created by Daniël Kamp on 08/04/2022.
//

#ifndef DONUT_STEREODELAY_H
#define DONUT_STEREODELAY_H

#include <Global.h>
#include <DSP/Buffer.h>
#include <DSP/DelayLine.h>
#include <System/ParameterStore.h>

class StereoDelay {
	public:
		StereoDelay(ParameterPool*, Buffer* left, Buffer* right);
		~StereoDelay();
	
		void process();
		
		Buffer* getLeftChannel();
		Buffer* getRightChannel();
	private:
		DelayLine *dlLeft, *dlRight;
};

#endif //DONUT_STEREODELAY_H
