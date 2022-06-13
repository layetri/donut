//
// Created by Daniël Kamp on 30/05/2022.
//

#include "Effect/Fold.h"

Fold::Fold (ParameterPool* params, Buffer* thru) {
	this->amount = params->get(p_FX_Fold_Amount, 0);
	this->gain = params->get(p_FX_Fold_Gain, 0);
	this->buffer = thru;
}

Fold::~Fold () {

}

void Fold::process() {
	double smp = (float) buffer->getCurrentSample() / (float) SAMPLE_MAX_VALUE;
//	smp = (smp >= SAMPLE_MAX_VALUE) * (FOLD_VAL - (smp - FOLD_VAL)) +
//		  (smp <= SAMPLE_MIN_VALUE) * ((-1 * FOLD_VAL) + (smp + FOLD_VAL)) +
//		  (smp > SAMPLE_MIN_VALUE && smp < SAMPLE_MAX_VALUE) *
//		  	(smp - (pow(smp, 3) / 3.0));
	
	smp = gain->value * 4.0 * (abs(0.25 * smp + 0.25 - round(0.25 * smp + 0.25)) - 0.25);
	buffer->write((amount->value * smp * SAMPLE_MAX_VALUE) + buffer->getCurrentSample() * (1.0f - amount->value));
}