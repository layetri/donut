//
// Created by Daniël Kamp on 29/11/2021.
//

#ifndef DONUT_CONTROLMAP_H
#define DONUT_CONTROLMAP_H

#include <string>
#include <map>
#include <cstdlib>
using namespace std;

struct Map {
	string name;
	map<string, int> values;
};

class ControlMap {
	public:
		ControlMap();
		~ControlMap();

		void addCC(int, string);
		void loadMap();
		void storeMap();
		int getCC(string name);

	private:
		Map* current_map;
};


#endif //DONUT_CONTROLMAP_H
