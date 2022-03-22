//
// Created by Daniël Kamp on 20/03/2022.
//

#ifndef DONUT_PARTICLELAYER_H
#define DONUT_PARTICLELAYER_H

// What is ParticleLayer?
// - 3 voices that are controlled by AI?
// - Granular engine controlled by AI?

#include "Global.h"
#include "Buffer.h"
#include "Voice.h"

class ParticleLayer {
	public:
		ParticleLayer(vector<Note*>* notes);
		~ParticleLayer();
		
		Buffer* getBuffer();
		
	private:
		Buffer* buffer;
		vector<Voice*> voices;
		vector<Note*>* notes;
};


#endif //DONUT_PARTICLELAYER_H
