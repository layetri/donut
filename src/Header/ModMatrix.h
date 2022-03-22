//
// Created by Daniël Kamp on 21/03/2022.
//

#ifndef DONUT_MODMATRIX_H
#define DONUT_MODMATRIX_H

#include <vector>
#include "Global.h"
#include "ParameterStore.h"
#include "Modulator.h"

struct ModDestination {
	Modulator* modulator;
	Parameter* parameter;
	uint8_t voice_id;
	float amount = 1.0;
};

class ModMatrix {
public:
	ModMatrix();
	~ModMatrix();
	
	void link(Parameter*, Modulator*, uint8_t);
	void process();
	void store(Modulator*);
	Modulator* get(string, uint8_t);
	
	vector<ModDestination*>* get();
private:
	vector<ModDestination*> connections;
	vector<Modulator*> modulators;
};


#endif //DONUT_MODMATRIX_H
