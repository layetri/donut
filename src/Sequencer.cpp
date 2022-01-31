//
// Created by Daniël Kamp on 10/11/2021.
//

#include "Header/Sequencer.h"

Sequencer::Sequencer(uint16_t len) {
	steps = new Buffer(len);
}

Sequencer::~Sequencer() {

}

void Sequencer::set(uint16_t note, uint8_t location) {
	steps->writeAtPosition((sample_t) note, location);
}

uint16_t Sequencer::next() {
	uint16_t step = steps->getCurrentSample();
	steps->tick();
	return step;
}