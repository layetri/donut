//
// Created by Daniël Kamp on 20/03/2022.
//

#include <Source/ParticleLayer.h>

ParticleLayer::ParticleLayer (vector<Note *> *notes) {
	this->notes = notes;
	
	for(int i = 0; i < 3; i++) {
//		voices.push_back(new Voice(voiceBuffer[i], params, tables, NUMBER_OF_VOICES + i));
	}
}