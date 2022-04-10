//
// Created by Daniël Kamp on 29/11/2021.
//

#include <System/ControlMap.h>

ControlMap::ControlMap() {
	current_map = new Map {"default_controls"};
	selected_controller = 1;

	// Map modwheel
	addCC(p_WS1_Detune, 1);
	addCC(p_WS2_Detune, 1);
	// Map knobs
	addCC(p_WS1_Harmonics, 21);
	addCC(p_WS2_Harmonics, 21);
	addCC(p_FM_Amount, 22);
	addCC(p_WT1_Shape, 23);
	addCC(p_WT2_Shape, 23);
	addCC(p_Filter_Cutoff, 24);
	addCC(p_Filter_Resonance, 25);
	addCC(p_LFO1_Rate, 26);
	addCC(p_ADSR1_Attack, 27);
	addCC(p_ADSR1_Release, 28);
	
	// Map faders
	// 41-48 -> 1-8, 7 -> master
	addCC(p_WS1_Amount, 41);
	addCC(p_WS2_Amount, 42);
	addCC(p_WT1_Amount, 43);
	addCC(p_WT2_Amount, 44);
	addCC(p_KS_Amount, 45);
	addCC(p_FX_Delay_DTLeft, 46);
	addCC(p_FX_Delay_FBLeft, 47);
	addCC(p_FX_Delay_Amount, 48);
	
	addCC(p_Master, 7);
}

ControlMap::~ControlMap() {}

void ControlMap::addCC(ParameterID pid, uint16_t cc, uint16_t channel) {
	current_map->values.push_back(new Control {pid, cc, channel});
}

void ControlMap::changeCC(ParameterID pid, uint16_t cc) {
	for(auto& v : current_map->values) {
		if(v->parameter == pid) {
			v->cc = cc;
		}
//		if(v->cc == cc && v->parameter != pid) {
//			v.second = 255;
//		}
	}
}

void ControlMap::loadMap() {

}

void ControlMap::storeMap() {}

int ControlMap::getCC(ParameterID pid) {
	for(auto& v : current_map->values) {
		if(v->parameter == pid) {
			return v->cc;
		}
	}
	return 0;
}

ParameterID ControlMap::getPID(uint16_t cc, uint16_t channel) {
	for(auto& v : current_map->values) {
		if(v->cc == cc && v->channel == channel) {
			return v->parameter;
		}
	}
	return p_NotFound;
}

void ControlMap::listControllers() {

}

void ControlMap::setController(uint input_num) {

}