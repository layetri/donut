//
// Created by Daniël Kamp on 28/03/2022.
//

#include <Source/Sampler.h>

SamplerVoice::SamplerVoice (Sampler *sampler, ParameterPool *params, uint8_t voice_id) : Source(params, voice_id) {
	this->sampler = sampler;
	this->transpose = params->get(p_Sampler_Transpose, voice_id);
	this->base_freq_dev = params->get(p_DEV_Sampler_Base, voice_id);
	
	prev_tick = 0;
	abs_position = 0;
	position = 0.0;
	increment = 1.0;
}

void SamplerVoice::process () {
	if(sampler->isReady() && playing) {
		auto smp = current_sample->sample->sample->getSample(abs_position);
		output->write((sample_t) (0.3 * smp + 0.7 * prev_tick));
		prev_tick = smp;
	}
}

void SamplerVoice::pitch(uint8_t midi_note) {
	current_sample = sampler->lookup(midi_note);
	if(retrigger) {
		position = current_sample->smp_start;
		abs_position = current_sample->smp_start;
	}
	increment = (samplerate / mtof(current_sample->key_root)) / mtof(127-(midi_note + 2 + (int8_t) transpose->value), 430.0f);// repitch sample
	playing = true;
}

void SamplerVoice::tick() {
	// TODO: add binding magic
	if(sampler->isReady() && playing) {
		if(current_sample->mode != oneshot || position < current_sample->smp_end) {
			position += increment;
			if(current_sample->mode != oneshot) {
				position = (position < current_sample->smp_end) * position +
				  (position >= current_sample->smp_end) * ((position - current_sample->smp_end) + current_sample->smp_start);
			}
			abs_position = floor(position);
		} else {
			playing = false;
		}
		
		output->tick();
	}
}

Sampler::Sampler(ParameterPool* params, SampleLibrary* lib, GUI* gui) {
	this->samples = lib->getSamples();
	this->library = lib;
	this->gui = gui;
}

void Sampler::addRegion (string sample, uint8_t start, uint8_t end, uint8_t root, uint smp_start, uint smp_end) {
	auto smp = library->get(sample);
	auto sz = (smp_end > 0) * smp_end + (smp_end == 0) * smp->sample->getSize();
	
	regions.push_back(new SamplerRegion{smp, smp_start, sz, start, end, root});
	loaded = true;
}

SamplerRegion* Sampler::lookup(uint8_t midi_note) {
	for(auto& r : regions) {
		if(midi_note >= r->key_start && midi_note <= r->key_end) {
			return r;
		}
	}
	return nullptr;
}

vector<SamplerRegion*>* Sampler::getRegions() {
	return &regions;
}

void Sampler::setRoot(string name, uint8_t key) {
	for(auto& r : regions) {
		if(r->sample->name == name) {
			r->key_root = key;
		}
	}
}

void Sampler::reset() {
	// Protect from crashing
	loaded = false;
	
	// Clear all regions
	regions.clear();
}