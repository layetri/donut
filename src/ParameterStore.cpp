//
// Created by Daniël Kamp on 21/01/2022.
//

#include "Header/ParameterStore.h"

ParameterPool::ParameterPool() {
	store(p_Harmonics, "harmonics", 0.5);
	store(p_Detune, "detune", 0.0);
	store(p_Detune_Range, "detune_range", 6.35f);

	store(p_AMP_Attack, "amp_attack", 0.0);
	store(p_AMP_Decay, "amp_decay", 0.0);
	store(p_AMP_Sustain, "amp_sustain", 1.0);
	store(p_AMP_Release, "amp_release", 0.0);

	store(p_MOD_Attack, "mod_attack", 0.0);
	store(p_MOD_Decay, "mod_decay", 0.0);
	store(p_MOD_Sustain, "mod_sustain", 1.0);
	store(p_MOD_Release, "mod_release", 0.0);

	store(p_Filter_Cutoff, "filter_cutoff", 1.0);
	store(p_Filter_Resonance, "filter_resonance", 0.0);
	store(p_Filter_Type, "filter_type", 0.0);
	store(p_Filter_KeyTrack, "filter_keytrack", 0.0);

	store(p_FM_Amount, "fm_amount", 0.77);
	store(p_FM_KeyTrack, "fm_keytrack", 0.5);

	store(p_LFO1_Rate, "lfo1_rate", 0.0);
}

ParameterPool::~ParameterPool() {}

void ParameterPool::set(ParameterID pid, float value) {
	get(pid)->value = value;
	get(pid)->base_value = value;
}

void ParameterPool::add(ParameterID pid, float value) {
	get(pid)->value = get(pid)->base_value + value;
}

void ParameterPool::load(map<string, float> params) {
	for(auto& p : params) {
		set(translate(p.first), p.second);
	}
}

Parameter* ParameterPool::get(ParameterID pid) {
	for(auto& p : parameters) {
		if(p->pid == pid) {
			return p;
		}
	}
}

ParameterID ParameterPool::translate(string key) {
	for(auto& p : parameters) {
		if(p->key == key) {
			return p->pid;
		}
	}
	return p_NotFound;
}

vector<Parameter*>* ParameterPool::getAll() {
	return &parameters;
}

float ParameterPool::value(ParameterID pid) {
	return get(pid)->value;
}

void ParameterPool::store(ParameterID pid, string key, float value) {
	parameters.push_back(new Parameter {pid, key, value});
}