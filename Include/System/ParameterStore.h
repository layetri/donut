//
// Created by Daniël Kamp on 21/01/2022.
//

#ifndef DONUT_PARAMETERSTORE_H
#define DONUT_PARAMETERSTORE_H
#include "../Global.h"
#include <vector>
#include <tuple>

using namespace std;

struct Parameter {
	ParameterID pid;
	uint8_t voice_id;
	float value;
	float base_value;
	
	float min = 0.0f, max = 1.0f;
	// TODO: Add scaling lambda with default range [0f, 1f]
	// TODO: Add isPublic flag
};

struct ParameterPreset {
	string key;
	uint8_t voice_id;
	float value;
};

struct ParameterDict {
	ParameterID pid;
	string key;
	string description = "Placeholder description.";
};

class ParameterPool {
	public:
		ParameterPool();
		~ParameterPool();

		Parameter* get(ParameterID pid, uint8_t voice_id);
		void set(ParameterID pid, uint8_t voice_id, float value);
		void add(ParameterID pid, uint8_t voice_id, float value);
		void store(ParameterID pid, uint8_t voice_id, string key, float value, float base_value=1.0, float min=0.0f, float max=1.0f);
		void load(vector<ParameterPreset*>* params);

		ParameterID translate(string key);
		string translate(ParameterID pid);

		float value(ParameterID pid, uint8_t voice_id);
		vector<Parameter*>* getAll();
		vector<ParameterDict*>* getDictionary();

	private:
		vector<Parameter*> parameters;
		vector<ParameterDict*> dictionary;
};


#endif //DONUT_PARAMETERSTORE_H
