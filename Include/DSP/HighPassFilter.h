//
// Created by Daniël Kamp on 09/04/2022.
//

#ifndef DONUT_HIGHPASSFILTER_H
#define DONUT_HIGHPASSFILTER_H

#include <Global.h>
#include <DSP/Filter.h>

class HighPassFilter : public Filter {
	public:
		HighPassFilter(Parameter* frequency, Buffer* input, Buffer* output);
		~HighPassFilter();
		
		void process();
		void handleFrequency();
	private:
		double b0, b1, b2, a0, a1, a2, q;
		Parameter* frequency;
		float old_f;
};


#endif //DONUT_HIGHPASSFILTER_H
