//
// Created by Daniël Kamp on 13/03/2022.
//

#include "Header/Tables.h"

Tables::Tables() {

}

Tables::~Tables() {
//	delete sine;
//	delete square;
//	delete triangle;
}

void Tables::generateWaveforms() {
	float frequency = 3.0; // Generate one period for highest precision
	uint16_t max_t = (uint16_t) (samplerate / frequency);
	float phase_step = frequency / samplerate;
	float phase = 0.0;

	square = new Buffer(max_t);
	sine = new Buffer(max_t);
	triangle = new Buffer(max_t);

	uint16_t n, j;
	sample_t tri_tmp;
	for(int i = 0; i < max_t; i++) {
		// Approximate triangle wave, thanks Fourier
		tri_tmp = 0;
		for(j = 1; j < 10; j++) {
			n = (2 * j) + 1;
			tri_tmp += (((sin(TWO_PI * n * phase) / (n*n)) * SAMPLE_MAX_VALUE));
		}

		// Write waveforms to tables
		square->write(((i < max_t / 2.0) * 2 * SAMPLE_MAX_VALUE) - SAMPLE_MAX_VALUE); // Generate squarest square wave (1/0)
		sine->write(sin(TWO_PI * phase) * SAMPLE_MAX_VALUE); // Generate sine wave
		triangle->write(tri_tmp); // Generate tri wave

		// Do the phase thingy
		phase = (phase + phase_step < 1.0) * (phase + phase_step);
		square->tick();
		sine->tick();
		triangle->tick();
	}
}

Buffer* Tables::getSquare() {
	return square;
}

Buffer* Tables::getSine() {
	return sine;
}

Buffer* Tables::getTriangle() {
	return triangle;
}