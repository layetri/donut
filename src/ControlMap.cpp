//
// Created by Daniël Kamp on 29/11/2021.
//

#include "Header/ControlMap.h"

ControlMap::ControlMap() {}

ControlMap::~ControlMap() {}

void ControlMap::addCC(int cc, string name) {
	this->current_map->values.insert(pair<string, int> (name, cc));
}

void ControlMap::loadMap() {}

void ControlMap::storeMap() {}

int ControlMap::getCC(string name) {
	for(auto itr = current_map->values.find(name); itr != current_map->values.end(); itr++) {
		return itr->second;
	}
	return 0;
}