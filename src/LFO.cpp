//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/LFO.h"

LFO::LFO (float frequency, Tables *tables, ParameterPool *params, uint8_t voice_id) : Modulator(params, voice_id) {
	wave = tables->getSine();
}

void LFO::process () {

}

void LFO::refresh() {

}

void LFO::tick() {
	position += frequency;
	position = (ceil(position) < wave->getSize()) * position + (ceil(position) >= wave->getSize()) * (position - wave->getSize());
	fl_position = floor(position);
}