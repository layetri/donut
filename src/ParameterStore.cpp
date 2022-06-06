//
// Created by Daniël Kamp on 21/01/2022.
//

#include <System/ParameterStore.h>

ParameterPool::ParameterPool() {
	for(int i = 0; i < NUMBER_OF_VOICES; i++) {
		store(p_ADSR1_Attack, i, "adsr1_attack", 2200.0, 2200.0, 0.0f, 220500.0f);
		store(p_ADSR1_Decay, i, "adsr1_decay", 2200.0, 2200.0, 0.0f, 220500.0f);
		store(p_ADSR1_Sustain, i, "adsr1_sustain", 1.0, 1.0);
		store(p_ADSR1_Release, i, "adsr1_release", 2200.0, 2200.0, 0.0f, 220500.0f);

		store(p_ADSR2_Attack, i, "adsr2_attack", 2200.0, 2200.0, 0.0f, 220500.0f);
		store(p_ADSR2_Decay, i, "adsr2_decay", 2200.0, 2200.0, 0.0f, 220500.0f);
		store(p_ADSR2_Sustain, i, "adsr2_sustain", 1.0, 1.0);
		store(p_ADSR2_Release, i, "adsr2_release", 2200.0, 2200.0, 0.0f, 220500.0f);

		store(p_Filter_Cutoff, i, "filter_cutoff", 16000.0, 16000.0, 1.0f, 16000.0f);
		store(p_Filter_Resonance, i, "filter_resonance", 0.5, 0.5);
		store(p_Filter_Type, i, "filter_type", 0.0, 0.0);
		store(p_Filter_KeyTrack, i, "filter_keytrack", 0.0, 0.0);

		store(p_FM_KeyTrack, i, "fm-keytrack", 0.5, 0.5);
		
		store(p_WT1_Amount, i, "wt1_amount", 0.0, 0.0);
		store(p_FM_Amount, i, "wt1_fm", 0.77, 0.77);
		store(p_WT1_Shape, i, "wt1_shape", 1.0, 1.0);
		store(p_WT1_Detune, i, "wt1_detune", 430.0, 430.0, 350.0f, 500.0f);
		store(p_WT1_BaseFrequency, i, "wt1_base_frequency", 440.0, 440.0);
		store(p_WT1_Transpose, i, "wt1_transpose", 0.0, 0.0, -12.0f, 12.0f);
		
		store(p_WT2_Amount, i, "wt2_amount", 0.0, 0.0);
		store(p_WT2_Shape, i, "wt2_shape", 1.0, 1.0);
		store(p_WT2_Detune, i, "wt2_detune", 430.0, 430.0, 350.0f, 500.0f);
		store(p_WT2_BaseFrequency, i, "wt2_base_frequency", 440.0, 440.0);
		store(p_WT2_Transpose, i, "wt2_transpose", 0.0, 0.0, -12.0f, 12.0f);
		
		store(p_WS1_Amount, i, "ws1_amount", 0.0, 0.0);
		store(p_WS1_BaseFrequency, i, "ws1_base_frequency", 440.0, 440.0);
		store(p_WS1_Harmonics, i, "ws1_harmonics", 2.0, 2.0, -16.0f, 16.0f);
		store(p_WS1_Detune, i, "ws1_detune", 430.0, 430.0, 430.0f, 450.0f);
		store(p_WS1_Detune_Range, i, "ws1_detune_range", 6.35f, 6.35f);
		store(p_WS1_Transpose, i, "ws1_transpose", 0.0, 0.0, -12.0f, 12.0f);
		
		store(p_WS2_Amount, i, "ws2_amount", 0.0, 0.0);
		store(p_WS2_BaseFrequency, i, "ws2_base_frequency", 440.0, 440.0);
		store(p_WS2_Harmonics, i, "ws2_harmonics", 2.0, 2.0, -16.0f, 16.0f);
		store(p_WS2_Detune, i, "ws2_detune", 430.0, 430.0, 430.0f, 450.0f);
		store(p_WS2_Detune_Range, i, "ws2_detune_range", 6.35f, 6.35f);
		store(p_WS2_Transpose, i, "ws2_transpose", 0.0, 0.0, -12.0f, 12.0f);
		
		store(p_KS_Amount, i, "ks_amount", 1.0, 0.0);
		store(p_KS_DelayTime, i, "ks_delay", 440.0, 440.0, 1.0f, 1000.0f);
		store(p_KS_Feedback, i, "ks_feedback", 0.999999, 0.999999, 0.1f, 1.05f);
		store(p_KS_FilterCutoff, i, "ks_dampening", 10000.0, 10000.0, 1.0f, 16000.0f);
		store(p_KS_Transpose, i, "ks_transpose", 0.0, 0.0, -12.0f, 12.0f);

		store(p_LFO1_Rate, i, "lfo1_rate", 1.0, 1.0, 0.01f, 100.0f);
		store(p_LFO1_Sync, i, "lfo1_sync", 0.0, 0.0);
		store(p_LFO2_Rate, i, "lfo2_rate", 1.0, 1.0, 0.01f, 100.0f);
		store(p_LFO2_Sync, i, "lfo2_sync", 0.0, 0.0);
		
		store(p_RND1_Rate, i, "rnd1_rate", 10.0, 10.0, 0.01f, 100.0f);
		store(p_RND1_Range, i, "rnd1_range", 1.0, 1.0);
		store(p_RND1_Sync, i, "rnd1_sync", 0.0, 0.0);
		store(p_RND1_Slew, i, "rnd1_slew", 0.5, 0.5);
		
		store(p_OutputHPF_Frequency, i, "output-hpf-frequency", 10.0f, 10.0f, 1.0f, 1000.0f);
		store(p_NotFound, i, "not-found", 0.0, 0.0);
		store(p_VoiceMaster, i, "voice-master", 1.0, 0.0);
		
		store(p_Sampler_Amount, i, "sampler_amount", 1.0, 1.0);
		store(p_Sampler_Transpose, i, "sampler_transpose", 0.0, 0.0);
		
		store(p_DEV_Sampler_Base, i, "sampler_dev_base", 430.0f, 430.0f, 400.0f, 500.0f);
		
		store(p_Particles_Amount, i, "particles_amount", 1.0, 1.0);
		store(p_Particles_Density, i, "particles_density", 1.0, 1.0);
		store(p_Particles_Shape, i, "particles_shape", 0.5, 0.5);
		store(p_Particles_Algorithm, i, "particles_algorithm", 0.5, 0.5);
		store(p_Particles_GrainSize, i, "particles_grainsize", 100.0, 100.0, 1, 100);
		store(p_Particles_Position, i, "particles_position", 3000.0, 3000.0, 0, 100);
	}
	
	store(p_FX_Delay_Amount, 0, "fxdelay_amount", 0.5, 0.5);
	store(p_FX_Delay_DTLeft, 0, "fxdelay_time_left", 8000, 8000, 100, 44100);
	store(p_FX_Delay_DTRight, 0, "fxdelay_time_right", 8000, 8000, 100, 44100);
	store(p_FX_Delay_FBLeft, 0, "fxdelay_feedback_left", 0.5, 0.5, 0.01, 0.99);
	store(p_FX_Delay_FBRight, 0, "fxdelay_feedback_right", 0.5, 0.5, 0.01, 0.99);
	
	store(p_FX_Fold_Amount, 0, "fxfold_amount", 0.5f, 0.5f, 0.0f, 1.0f);
	store(p_FX_Fold_Gain, 0, "fxfold_gain", 1.0f, 1.0f, 0.5f, 1.5f);
	
	store(p_Master, 0, "master", 1.0, 1.0);
	store(p_BPM, 0, "bpm", 120.0, 120.0, 60.0f, 240.0f);
}

ParameterPool::~ParameterPool() {}

void ParameterPool::set(ParameterID pid, uint8_t voice_id, float value) {
	auto par = get(pid, voice_id);
	par->value = clamp(value, par->min, par->max);
	par->base_value = clamp(value, par->min, par->max);
}

void ParameterPool::add(ParameterID pid, uint8_t voice_id, float value) {
	auto par = get(pid, voice_id);
	par->value = clamp(par->base_value + value, par->min, par->max);
}

void ParameterPool::load(vector<ParameterPreset>* params) {
	for(auto& p : *params) {
		set(translate(p.key), p.voice_id, p.value);
	}
}

Parameter* ParameterPool::get(ParameterID pid, uint8_t voice_id) {
	for(auto& p : parameters) {
		if(p.pid == pid && p.voice_id == voice_id) {
			return &p;
		}
	}
	return &parameters[0];
}

ParameterID ParameterPool::translate(string key) {
	for(auto& p : dictionary) {
		if(p.key == key) {
			return p.pid;
		}
	}
	return p_NotFound;
}

string ParameterPool::translate(ParameterID pid) {
	for(auto& p : dictionary) {
		if(p.pid == pid) {
			return p.key;
		}
	}
	return "";
}

vector<Parameter>* ParameterPool::getAll() {
	return &parameters;
}
vector<ParameterDict>* ParameterPool::getDictionary() {
	return &dictionary;
}

float ParameterPool::value(ParameterID pid, uint8_t voice_id) {
	return get(pid, voice_id)->value;
}

void ParameterPool::store(ParameterID pid, uint8_t voice_id, string key, float value, float base_value, float min, float max) {
	if(!voice_id) {
		dictionary.push_back(ParameterDict {pid, key});
	}
	parameters.push_back(Parameter {pid, voice_id, value, base_value, min, max});
}