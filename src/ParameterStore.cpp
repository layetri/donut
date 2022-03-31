//
// Created by Daniël Kamp on 21/01/2022.
//

#include "Header/ParameterStore.h"

ParameterPool::ParameterPool() {
	for(int i = 0; i < NUMBER_OF_VOICES; i++) {
		store(p_ADSR1_Attack, i, "adsr1_attack", 2200.0);
		store(p_ADSR1_Decay, i, "adsr1_decay", 2200.0);
		store(p_ADSR1_Sustain, i, "adsr1_sustain", 1.0);
		store(p_ADSR1_Release, i, "adsr1_release", 2200.0);

		store(p_ADSR2_Attack, i, "adsr2_attack", 2200.0);
		store(p_ADSR2_Decay, i, "adsr2_decay", 2200.0);
		store(p_ADSR2_Sustain, i, "adsr2_sustain", 1.0);
		store(p_ADSR2_Release, i, "adsr2_release", 2200.0);

		store(p_Filter_Cutoff, i, "filter_cutoff", 16000.0);
		store(p_Filter_Resonance, i, "filter_resonance", 0.5);
		store(p_Filter_Type, i, "filter_type", 0.0);
		store(p_Filter_KeyTrack, i, "filter_keytrack", 0.0);

		store(p_FM_Amount, i, "fm_amount", 0.77);
		store(p_FM_KeyTrack, i, "fm_keytrack", 0.5);

		store(p_WT1_Amount, i, "wt1_amount", 0.0, 0.0);
		store(p_WT1_Shape, i, "wt1_shape", 1.0);
		store(p_WT1_Detune, i, "wt1_detune", 430.0);
		store(p_WT1_BaseFrequency, i, "wt1_base_frequency", 440.0);
		store(p_WT1_Transpose, i, "wt1_transpose", 0.0);
		
		store(p_WT2_Amount, i, "wt2_amount", 0.0, 0.0);
		store(p_WT2_Shape, i, "wt2_shape", 1.0);
		store(p_WT2_Detune, i, "wt2_detune", 430.0);
		store(p_WT2_BaseFrequency, i, "wt2_base_frequency", 440.0);
		store(p_WT2_Transpose, i, "wt2_transpose", 0.0);
		
		store(p_WS1_Amount, i, "ws1_amount", 0.0, 0.0);
		store(p_WS1_BaseFrequency, i, "ws1_base_frequency", 440.0);
		store(p_WS1_Harmonics, i, "ws1_harmonics", 2.0);
		store(p_WS1_Detune, i, "ws1_detune", 430.0);
		store(p_WS1_Detune_Range, i, "ws1_detune_range", 6.35f);
		store(p_WS1_Transpose, i, "ws1_transpose", 0.0);
		
		store(p_WS2_Amount, i, "ws2_amount", 1.0, 0.0);
		store(p_WS2_BaseFrequency, i, "ws2_base_frequency", 440.0);
		store(p_WS2_Harmonics, i, "ws2_harmonics", 2.0);
		store(p_WS2_Detune, i, "ws2_detune", 430.0);
		store(p_WS2_Detune_Range, i, "ws2_detune_range", 6.35f);
		store(p_WS2_Transpose, i, "ws2_transpose", 0.0);
		
		store(p_KS_Amount, i, "ks_amount", 1.0, 0.0);
		store(p_KS_DelayTime, i, "ks_delay", 440.0);
		store(p_KS_Feedback, i, "ks_feedback", 0.999999);
		store(p_KS_FilterCutoff, i, "ks_dampening", 0.5);
		store(p_KS_Transpose, i, "ks_transpose", 0.0);

		store(p_LFO1_Rate, i, "lfo1_rate", 1.0);
		store(p_LFO2_Rate, i, "lfo2_rate", 1.0);
		store(p_NotFound, i, "not_found", 0.0);
		store(p_VoiceMaster, i, "voice_master", 1.0, 0.0);
	}
	
	store(p_Master, 0, "master", 1.0);
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

void ParameterPool::store(ParameterID pid, uint8_t voice_id, string key, float value, float base_value) {
	if(!voice_id) {
		dictionary.push_back(new ParameterDict {pid, key});
	}
	parameters.push_back(new Parameter {pid, voice_id, value, base_value});
}