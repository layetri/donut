//
// Created by Daniël Kamp on 29/11/2021.
//

#ifndef DONUT_CONTROLMAP_H
#define DONUT_CONTROLMAP_H

#include "Global.h"
#include "ParameterStore.h"
#include <string>
#include <unordered_map>
#include <cstdlib>

using namespace std;

struct Map {
	string name;
	unordered_map<ParameterID, uint16_t> values;
};

class ControlMap {
	public:
		ControlMap();
		~ControlMap();

		void addCC(uint16_t, ParameterID);
		void changeCC(ParameterID, uint16_t);
		void loadMap();
		void storeMap();
		int getCC(ParameterID pid);
		ParameterID getPID(uint16_t cc);

	private:
		Map* current_map;
};


#endif //DONUT_CONTROLMAP_H
