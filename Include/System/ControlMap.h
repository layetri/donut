//
// Created by Daniël Kamp on 29/11/2021.
//

#ifndef DONUT_CONTROLMAP_H
#define DONUT_CONTROLMAP_H

#include <Global.h>
#include "ParameterStore.h"
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
	string name;
	vector<Control*> values;
};

class ControlMap {
	public:
		ControlMap();
		~ControlMap();

		void addCC(ParameterID, uint16_t, uint16_t channel=0);
		void changeCC(ParameterID, uint16_t);
		void loadMap();
		void storeMap();
		int getCC(ParameterID pid);
		ParameterID getPID(uint16_t cc, uint16_t channel=0);

		void setController(uint);
		void listControllers();

	private:
		Map* current_map;
		uint selected_controller;

};


#endif //DONUT_CONTROLMAP_H
