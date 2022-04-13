//
// Created by Daniël Kamp on 28/03/2022.
//

#ifndef DONUT_SAMPLER_H
#define DONUT_SAMPLER_H

#include <Global.h>
#include <Source/Source.h>
#include <System/SampleLibrary.h>

enum RepeatMode {
	oneshot,
	bounce,
	circular
};

struct SamplerRegion {
	Sample* sample;
	uint smp_start; // Sample position to start playback from, inclusive
	uint smp_end; // Sample position to end playback on
	uint8_t key_start = 0; // MIDI key where the region starts, inclusive
	uint8_t key_end = 127; // MIDI key where the region ends, inclusive
	uint8_t key_root = 60;
	RepeatMode mode = oneshot;
};

// The base Sampler class that contains region assignments etc
class Sampler {
	public:
		Sampler(ParameterPool*, SampleLibrary* lib);
		~Sampler() = default;
		
		void addRegion(string);
		void setRoot(string, uint8_t);
		
		SamplerRegion* lookup(uint8_t);
		vector<SamplerRegion*>* getRegions();
		bool isReady() {return loaded;};
	private:
		vector<Sample*>* samples;
		vector<SamplerRegion*> regions;
		SampleLibrary* library;
		Parameter *start, *length, *speed, *direction;
		
		RepeatMode repeat;
		uint position;
		bool loaded = false;
};

// The Sampler instance that exists on every voice
class SamplerVoice : public Source {
	public:
		SamplerVoice(Sampler*, ParameterPool*, uint8_t);
		~SamplerVoice() = default;
	
		void process() override;
		void pitch(uint8_t midi_note) override;
		void tick() override;
	private:
		Sampler* sampler;
		SamplerRegion* current_sample;
		Parameter *transpose;
		
		sample_t prev_tick; // Keep 1 sample of output history, for smoothing
		uint abs_position;
		float increment, position;
		bool retrigger = true;
		bool playing = false;
};

#endif //DONUT_SAMPLER_H
