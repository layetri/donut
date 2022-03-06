//
// Created by Daniël Kamp on 21/01/2022.
//

#ifndef DONUT_PARAMETERSTORE_H
#define DONUT_PARAMETERSTORE_H
#include "Global.h"
#include <vector>
#include <map>

using namespace std;

struct Parameter {
	ParameterID pid;
	string key;
	float value;
	float base_value;
	vector<float> lfo_amount;
};

class ParameterPool {
	public:
		ParameterPool();
		~ParameterPool();

		Parameter* get(ParameterID pid);
		void set(ParameterID pid, float value);
		void add(ParameterID pid, float value);
		void store(ParameterID pid, string key, float value);
		void load(map<string, float> params);
		ParameterID translate(string key);
		float value(ParameterID pid);
		vector<Parameter*>* getAll();
	private:
		vector<Parameter*> parameters;
};


#endif //DONUT_PARAMETERSTORE_H
