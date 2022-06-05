//
// Created by Daniël Kamp on 21/03/2022.
//

#ifndef DONUT_MODMATRIX_H
#define DONUT_MODMATRIX_H

#include <vector>
#include <Global.h>
#include <System/ParameterStore.h>
#include <Modulator/Modulator.h>

struct ModDestination {
	Modulator* modulator;
	Parameter* parameter;
	uint8_t voice_id;
	float amount = 1.0;
};

struct ModDict {
	ModID mid;
	string key;
};

class ModMatrix {
public:
	ModMatrix(ParameterPool* parameters);
	~ModMatrix();
	
	void link(Parameter*, Modulator*, uint8_t, float amount=1.0);
	void process();
	void store(Modulator*);
	void setOrCreate(ModID, ParameterID, float);
	bool clearAll();
	
	Modulator* get(string, uint8_t);
	Modulator* get(ModID, uint8_t);
	vector<ModDestination*>* get();
	ModDestination* get(Modulator*, Parameter*, uint8_t);
	vector<Modulator*>* getMods();
	vector<ModDict>* getDict();
	vector<ModDestination*> getByVoice(uint8_t);
private:
	vector<ModDestination*> connections;
	vector<Modulator*> modulators;
	vector<ModDict> dictionary;
	ParameterPool* parameters;
};


#endif //DONUT_MODMATRIX_H
