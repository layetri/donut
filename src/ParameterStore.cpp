//
// Created by Daniël Kamp on 21/01/2022.
//

#include "Header/ParameterStore.h"

ParameterPool::ParameterPool() {


	for(int i = 0; i < NUMBER_OF_VOICES; i++) {
		store(p_AMP_Attack, i, "amp_attack", 0.0);
		store(p_AMP_Decay, i, "amp_decay", 0.0);
		store(p_AMP_Sustain, i, "amp_sustain", 1.0);
		store(p_AMP_Release, i, "amp_release", 0.0);

		store(p_MOD_Attack, i, "mod_attack", 0.0);
		store(p_MOD_Decay, i, "mod_decay", 0.0);
		store(p_MOD_Sustain, i, "mod_sustain", 1.0);
		store(p_MOD_Release, i, "mod_release", 0.0);

		store(p_Filter_Cutoff, i, "filter_cutoff", 1.0);
		store(p_Filter_Resonance, i, "filter_resonance", 0.0);
		store(p_Filter_Type, i, "filter_type", 0.0);
		store(p_Filter_KeyTrack, i, "filter_keytrack", 0.0);

		store(p_FM_Amount, i, "fm_amount", 0.77);
		store(p_FM_KeyTrack, i, "fm_keytrack", 0.5);

		store(p_WT_Shape, i, "wt_shape", 1.0);
		
		store(p_WS_BaseFrequency, i, "ws_base_frequency", 1.0);
		store(p_WS_Harmonics, i, "harmonics", 0.5);
		store(p_WS_Detune, i, "detune", 430.0);
		store(p_WS_Detune_Range, i, "detune_range", 6.35f);

		store(p_LFO1_Rate, i, "lfo1_rate", 0.0);
		store(p_NotFound, i, "not_found", 0.0);
	}
}

ParameterPool::~ParameterPool() {}

void ParameterPool::set(ParameterID pid, uint8_t voice_id, float value) {
	get(pid, voice_id)->value = value;
	get(pid, voice_id)->base_value = value;
}

void ParameterPool::add(ParameterID pid, uint8_t voice_id, float value) {
	get(pid, voice_id)->value = get(pid, voice_id)->base_value + value;
}

void ParameterPool::load(vector<ParameterPreset*>* params) {
	for(auto& p : *params) {
		set(translate(p->key), p->voice_id, p->value);
	}
}

Parameter* ParameterPool::get(ParameterID pid, uint8_t voice_id) {
	for(auto& p : parameters) {
		if(p->pid == pid && p->voice_id == voice_id) {
			return p;
		}
	}
}

ParameterID ParameterPool::translate(string key) {
	for(auto& p : dictionary) {
		if(p->key == key) {
			return p->pid;
		}
	}
	return p_NotFound;
}

string ParameterPool::translate(ParameterID pid) {
	for(auto& p : dictionary) {
		if(p->pid == pid) {
			return p->key;
		}
	}
	return "";
}

vector<Parameter*>* ParameterPool::getAll() {
	return &parameters;
}
vector<ParameterDict*>* ParameterPool::getDictionary() {
	return &dictionary;
}

float ParameterPool::value(ParameterID pid, uint8_t voice_id) {
	return get(pid, voice_id)->value;
}

void ParameterPool::store(ParameterID pid, uint8_t voice_id, string key, float value) {
	if(!voice_id) {
		dictionary.push_back(new ParameterDict {pid, key});
	}
	parameters.push_back(new Parameter {pid, voice_id, value});
}