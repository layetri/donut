//
// Created by Daniël Kamp on 08/04/2022.
//

#include <Effect/StereoDelay.h>

StereoDelay::StereoDelay (ParameterPool* p, Buffer* in_l, Buffer* in_r) {
	dlLeft = new DelayLine(p->get(p_FX_Delay_DTLeft, 0), p->get(p_FX_Delay_FBLeft, 0), in_l);
	dlRight = new DelayLine(p->get(p_FX_Delay_DTRight, 0), p->get(p_FX_Delay_FBRight, 0), in_r);
}
StereoDelay::~StereoDelay () {}

void StereoDelay::process () {
	// TODO: add highpass filter
	dlLeft->process();
	dlRight->process();
}

Buffer* StereoDelay::getLeftChannel() {
	return dlLeft->getBuffer();
}

Buffer* StereoDelay::getRightChannel() {
	return dlRight->getBuffer();
}