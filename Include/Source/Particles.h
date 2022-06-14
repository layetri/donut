//
// Created by Daniël Kamp on 20/03/2022.
//

#ifndef DONUT_PARTICLES_H
#define DONUT_PARTICLES_H

#define MAX_NUM_PARTICLES 16
#define CHUNKING 6

#include <algorithm>
#include <queue>
#include <deque>

#include <Global.h>
#include <Source/Source.h>
#include <System/SampleLibrary.h>

enum ParticleShape {
	env_Quadratic,
	env_Linear,
	env_Boxcar
};

enum GranulationAlgorithm {
	ga_DivideDown,
	ga_Deterministic,
	ga_RandomStart,
	ga_Note
};

// How to shape:
// - sample = shape * (shape1 calc) + 1.0-shape * (shape2 calc)

struct GrainWindow {
	ParticleShape shape = env_Linear;
	
	float amplitude = 0.0f;
	// For quadratic
	float slope = 0.0, curve = 0.0f;
	// For linear
	float lin_increment = 0.0f, lin_decrement = 0.0f;
	uint midpoint = 0;
};

struct Particle {
	Sample** sample{};
	Parameter *shape{}, *playhead{};
	uint start = 0;
	uint length = 50;
	float speed = 1.0;
	uint position = 0;
	bool active = false;
	bool schedule_for_deletion_ = false;
	bool in_queue_ = false;
	GrainWindow window;
	
	void prerender() {
		// Set flags
		schedule_for_deletion_ = false;
		in_queue_ = false;
		
		// Reset position
		position = 0;
		float d1, d2;
		
		switch(window.shape) {
			case env_Quadratic:
				// Prepare windowing values for runtime
				window.amplitude = 0.0f;
				d1 = 1.0f / (float) length;
				d2 = d1 * d1;
				window.slope = 4.0f * 0.8f * (d1 - d2);
				window.curve = -8.0f * 0.8f * d2;
				break;
			case env_Linear:
				window.midpoint = (uint) (length * shape->value);
				window.lin_increment = 1.0f / (float) window.midpoint;
				window.lin_decrement = 1.0f / (float) (length - window.midpoint);
				break;
			case env_Boxcar:
				
				break;
		}
	}
	
	void tick() {
		if(position < length) {
			position++;
		} else {
			schedule_for_deletion_ = true;
		}
	}
	
	sample_t getSample() {
		// Get the current sample from memory
		sample_t smp = (*sample)->sample->getSample(floor(playhead->value + position * speed));
		switch(window.shape) {
			case env_Quadratic:
				// Update windowing values
				window.amplitude += window.slope;
				window.slope += window.curve;
				break;
			case env_Linear:
				if(position < window.midpoint) {
					window.amplitude += window.lin_increment;
				} else {
					window.amplitude -= window.lin_decrement;
				}
				break;
			case env_Boxcar:
				
				break;
		}
		
		// Apply windowing and return
		return smp * window.amplitude;
	}
};

class Particles {
	public:
		Particles(SampleLibrary* library);
		~Particles();
		
		void swap(const string&, int root_key=60);
		Sample** getSample();
		float getBaseFrequency();
		uint getMaxPosition();
		bool hasSampleChanged();
	private:
		SampleLibrary* library;
		Sample* sample;
		float base_frequency = 0.0;
		bool sample_changed_ = false;
};

class ParticleVoice : public Source {
public:
	ParticleVoice(Particles* engine, ParameterPool* params, uint8_t voice_id);
	~ParticleVoice();

	void process() override;
	void tick() override;
	void pitch(uint8_t) override;
	void block(size_t block_size) override;
	
	vector<Particle*> getInactiveParticles();
private:
	Particles* engine;
	queue<Particle*> next_;
	deque<Particle*> active_particles;
	
	vector<Particle*> particles;
	Parameter *shape, *density, *algorithm, *grain_size, *playhead;
	
	uint last_block_offset_ = 0;
	uint block_size = 0;
	uint block_counter = 0;
	uint aggregate_counter_ = CHUNKING;
	float frequency = 440.0f;
};

#endif //DONUT_PARTICLES_H
