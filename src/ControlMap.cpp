//
// Created by Daniël Kamp on 29/11/2021.
//

#include <System/ControlMap.h>

ControlMap::ControlMap(ParameterPool* pool) {
	this->pool = pool;
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
	addCC(p_Particles_GrainSize, 25);
	addCC(p_Particles_Density, 26);
	addCC(p_ADSR1_Attack, 27);
	addCC(p_ADSR1_Release, 28);
	
	// Map faders
	// 41-48 -> 1-8, 7 -> master
	addCC(p_WS1_Amount, 41);
	addCC(p_WS2_Amount, 42);
	addCC(p_WT1_Amount, 43);
	addCC(p_WT2_Amount, 44);
	addCC(p_KS_Amount, 45);
	addCC(p_Sampler_Amount, 46);
	addCC(p_Particles_Amount, 47);
	addCC(p_FX_Delay_Amount, 48);
	
	addCC(p_Master, 7);
}

ControlMap::~ControlMap() {}

void ControlMap::addCC(ParameterID pid, uint16_t cc, uint16_t channel) {
	current_map->values.push_back(new Control {pid, cc, channel});
}

void ControlMap::changeCC(ParameterID pid, uint16_t cc) {
	for(auto& v : current_map->values) {
		if(v->cc == cc) {
			v->parameter = pid;
		}
	}
}

void ControlMap::loadMap(const string& name) {
	filesystem::path path = filesystem::current_path() / ".donut_runtime/controls";
	
	for (const auto & entry : filesystem::directory_iterator(path)) {
		string filename = entry.path().filename().string();
		
		if (filename == name + ".donutcontrols") {
			ifstream file(entry.path().c_str());
			nlohmann::json filecontents;
			file >> filecontents;
			
			reset();
			
			for(auto& c : filecontents["controls"]) {
				addCC(pool->translate((string) c["parameter"]), c["cc"], c["channel"]);
			}
		}
	}
}

void ControlMap::storeMap(const string& name) {
	// Store a controls map to /.donut_runtime/controls as .donutcontrols
	filesystem::path path = filesystem::current_path() / ".donut_runtime/controls";
	ofstream out(path.append(name + ".donutcontrols"));
	
	nlohmann::json filecontents;
	filecontents["controls"] = {};
	
	for(auto& v : current_map->values) {
		string key = pool->translate(v->parameter);
		filecontents["controls"].push_back({
			{"cc", v->cc},
			{"channel", v->channel},
			{"parameter", key}
		});
	}
	
	out << setw(4) << filecontents << endl;
	out.close();
}

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

vector<ParameterID> ControlMap::getPIDs(uint16_t cc, uint16_t channel) {
	vector<ParameterID> ret_arr;
	
	for(auto& v : current_map->values) {
		if(v->cc == cc && v->channel == channel) {
			ret_arr.push_back(v->parameter);
		}
	}
	
	return ret_arr;
}

void ControlMap::reset() {
	current_map->values.clear();
}

void ControlMap::listControllers() {

}

void ControlMap::setController(uint input_num) {

}