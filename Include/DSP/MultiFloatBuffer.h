//
// Created by Daniël Kamp on 06/12/2021.
//

#ifndef DONUT_MULTIFLOATBUFFER_H
#define DONUT_MULTIFLOATBUFFER_H

#include "FloatBuffer.h"
#include "../Global.h"

#include <vector>

using namespace std;

class MultiFloatBuffer : public FloatBuffer {
	public:
		MultiFloatBuffer(int length, int buffers);
		~MultiFloatBuffer();

		float getMultiSample(int);
		void multiTick(int);
		void setMultiPosition(int, int);
		int getMultiPosition(int);

	private:
		vector<int> readheads;
};


#endif //DONUT_MULTIFLOATBUFFER_H
