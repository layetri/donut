//
// Created by Daniël Kamp on 28/03/2022.
//

#include <Source/Sampler.h>

SamplerVoice::SamplerVoice (Sampler *sampler, ParameterPool *params, uint8_t voice_id) : Source(params, voice_id) {
	this->sampler = sampler;
	this->transpose = params->get(p_Sampler_Transpose, voice_id);
	
	prev_tick = 0;
	abs_position = 0;
	position = 0.0;
	increment = 1.0;
}

void SamplerVoice::process () {
	if(sampler->isReady() && playing) {
		output->write((sample_t) (0.3 * current_sample->sample->sample->getSample(abs_position) + 0.7 * prev_tick));
		prev_tick = current_sample->sample->sample->getSample(abs_position);
	}
}

void SamplerVoice::pitch(uint8_t midi_note) {
	current_sample = sampler->lookup(midi_note);
	if(retrigger) {
		position = current_sample->smp_start;
		abs_position = current_sample->smp_start;
	}
	increment = (samplerate / mtof(current_sample->key_root)) / mtof(127-(midi_note + 2 + (uint8_t) transpose->value), 440.0);// repitch sample
	playing = true;
}

void SamplerVoice::tick() {
	// TODO: add binding magic
	if(sampler->isReady() && playing) {
		if(current_sample->mode != oneshot || position < current_sample->smp_end) {
			position += increment;
			position = (current_sample->mode != oneshot) * ((position < current_sample->smp_end) * position + (position >= current_sample->smp_end) * ((position - current_sample->smp_end) + current_sample->smp_start)) + (current_sample->mode == oneshot) * position;
			abs_position = floor(position);
		} else {
			playing = false;
		}
		
		output->tick();
	}
}

Sampler::Sampler(ParameterPool* params, SampleLibrary* lib) {
	this->samples = lib->getSamples();
	this->library = lib;
}

void Sampler::addRegion(string sample) {
	auto smp = library->get(sample);
	regions.push_back(new SamplerRegion {smp, 0, smp->sample->getSize()});
	loaded = true;
}

SamplerRegion* Sampler::lookup(uint8_t midi_note) {
	for(auto& r : regions) {
		if(midi_note >= r->key_start && midi_note <= r->key_end) {
			return r;
		}
	}
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