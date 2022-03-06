//
// Created by Daniël Kamp on 29/11/2021.
//

#include "Header/ControlMap.h"

ControlMap::ControlMap() {
	current_map = new Map {"default_controls"};
	selected_controller = 1;

	// Map modwheel
	addCC(1, p_Detune);
	// Map knobs
	addCC(21, p_Harmonics);
	addCC(22, p_FM_Amount);
	addCC(23, p_Filter_Cutoff);
	addCC(24, p_Filter_Resonance);
	addCC(25, p_AMP_Attack);
	addCC(26, p_AMP_Decay);
	addCC(27, p_AMP_Sustain);
	addCC(28, p_AMP_Release);
}

ControlMap::~ControlMap() {}

void ControlMap::addCC(uint16_t cc, ParameterID pid) {
	current_map->values.emplace(pid, cc);
}

void ControlMap::changeCC(ParameterID pid, uint16_t cc) {
	for(auto& v : current_map->values) {
		if(v.first == pid) {
			v.second = cc;
		}
		if(v.second == cc && v.first != pid) {
			v.second = 255;
		}
	}
}

void ControlMap::loadMap() {

}

void ControlMap::storeMap() {}

int ControlMap::getCC(ParameterID pid) {
	for(auto& v : current_map->values) {
		if(v.first == pid) {
			return v.second;
		}
	}
	return 0;
}

ParameterID ControlMap::getPID(uint16_t cc) {
	for(auto& v : current_map->values) {
		if(v.second == cc) {
			return v.first;
		}
	}
	return p_NotFound;
}

void ControlMap::listControllers() {

}

void ControlMap::setController(uint input_num) {

}