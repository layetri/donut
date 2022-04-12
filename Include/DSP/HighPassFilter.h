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

	private:
		Parameter *frequency;
};


#endif //DONUT_HIGHPASSFILTER_H
