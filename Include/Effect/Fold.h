//
// Created by Daniël Kamp on 30/05/2022.
//

#ifndef DONUT_FOLD_H
#define DONUT_FOLD_H

#define FOLD_VAL (SAMPLE_MAX_VALUE * (2.0f/3.0f))

#include <Global.h>
#include <DSP/Buffer.h>
#include <System/ParameterStore.h>

class Fold {
public:
	Fold (ParameterPool* params, Buffer* thru);
	~Fold ();

	void process();
private:
	Parameter *amount, *gain;
	Buffer *buffer;
};


#endif //DONUT_FOLD_H
