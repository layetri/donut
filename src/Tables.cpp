//
// Created by Daniël Kamp on 13/03/2022.
//

#include "Header/Tables.h"

Tables::Tables() {

}

Tables::~Tables() {
	delete sine;
	delete square;
	delete triangle;
}

void Tables::generateWaveforms() {
	uint16_t max_t = (uint16_t) (samplerate / TABLE_FREQUENCY);
	float phase_step = TABLE_FREQUENCY / samplerate;
	float phase = 0.0;

	square = new Buffer(max_t);
	sine = new Buffer(max_t);
	triangle = new Buffer(max_t);

	uint16_t n, j;
	for(int i = 0; i < max_t; i++) {
		// Approximate triangle wave, thanks Fourier
		square->flush();
		triangle->flush();
		for(j = 0; j < 8; j++) {
			n = (2 * j) + 1;
			triangle->writeAddition(((sin(TWO_PI * n * phase) / (n*n)) * SAMPLE_MAX_VALUE));
			square->writeAddition(((sin(TWO_PI * j * phase) / (2*j-1)) * SAMPLE_MAX_VALUE));
		}

		// Write waveforms to tables
//		square->write(((i % max_t < max_t / 2.0) * 2 * SAMPLE_MAX_VALUE) - SAMPLE_MAX_VALUE); // Generate squarest square wave (1/0)
//		square->write(sqr_tmp); // Generate approximate square wave (fourier)
		sine->write(sin(TWO_PI * phase) * SAMPLE_MAX_VALUE); // Generate sine wave
//		triangle->write(wave_tmp); // Generate tri wave

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