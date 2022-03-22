//
// Created by Daniël Kamp on 21/03/2022.
//

#ifndef DONUT_CONCATENGINE_H
#define DONUT_CONCATENGINE_H

#include "Global.h"
#include "Buffer.h"

// Corpus: a multi-playhead buffer implementation
// Loads a .wav file of the entire corpus + a .txt file of the corpus metadata
//class Corpus : public Buffer {
//	public:
//		Corpus();
//		~Corpus();
//};

class ConcatEngine {
	public:
		ConcatEngine();
		~ConcatEngine();
	
	private:
		Buffer* corpus;
		
};


#endif //DONUT_CONCATENGINE_H
