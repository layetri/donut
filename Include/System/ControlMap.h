//
// Created by Daniël Kamp on 29/11/2021.
//

#ifndef DONUT_CONTROLMAP_H
#define DONUT_CONTROLMAP_H

#include <Global.h>
#include <System/ParameterStore.h>
#include <ext/json.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

struct Control {
	ParameterID parameter;
	uint16_t cc;
	uint16_t channel = 0;
};

struct Map {
	string name = "default_map";
	vector<Control*> values;
};

class ControlMap {
	public:
		ControlMap(ParameterPool*);
		~ControlMap();

		void addCC(ParameterID, uint16_t, uint16_t channel=0);
		void changeCC(ParameterID, uint16_t);
		void loadMap(const string& name);
		void storeMap(const string& name);
		void reset();
		int getCC(ParameterID pid);
		ParameterID getPID(uint16_t cc, uint16_t channel=0);
		vector<ParameterID> getPIDs(uint16_t, uint16_t channel=0);

	private:
		ParameterPool* pool;
		Map current_map;

};


#endif //DONUT_CONTROLMAP_H
