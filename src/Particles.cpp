//
// Created by Daniël Kamp on 20/03/2022.
//

#include <Source/Particles.h>

Particles::Particles(SampleLibrary* library) {
	this->library = library;
	this->sample = library->getDefaultSample();
	base_frequency = Source::mtof(50, 430.0f);
}

Particles::~Particles () {

}

void Particles::swap(const string& name, int root_key) {
	sample = library->get(name);
	base_frequency = Source::mtof(root_key, 440.0f);
	sample_changed_ = true;
}

float Particles::getBaseFrequency () {
	return base_frequency;
}

uint Particles::getMaxPosition () {
	return this->sample->sample->getSize();
}

bool Particles::hasSampleChanged () {
	return sample_changed_;
}

Sample** Particles::getSample () {
	return &sample;
}

// End of Particles global engine class
// Start of ParticleVoice Source component

ParticleVoice::ParticleVoice (Particles* engine, ParameterPool* params, uint8_t voice_id) : Source(params, voice_id) {
	this->engine = engine;
	this->shape = params->get(p_Particles_Shape, voice_id);
	this->density = params->get(p_Particles_Density, voice_id);
	this->algorithm = params->get(p_Particles_Algorithm, voice_id);
	this->grain_size = params->get(p_Particles_GrainSize, voice_id);
	this->playhead = params->get(p_Particles_Position, voice_id);
	
	grain_size->max = engine->getMaxPosition();
	playhead->max = engine->getMaxPosition() - grain_size->value;
	
	for(int i = 0; i < MAX_NUM_PARTICLES; i++) {
		this->particles.push_back(new Particle {engine->getSample(), shape, playhead});
	}
}

ParticleVoice::~ParticleVoice () {
	for(auto& p : particles) {
		delete p;
	}
}

void ParticleVoice::block(size_t ext_block_size) {
	// This is where we do all pre-calculation for the next N blocks
	block_size += ext_block_size;
	if(engine->hasSampleChanged()) {
		grain_size->max = engine->getMaxPosition();
		playhead->max = engine->getMaxPosition() - grain_size->value;
	}
	
	// Apply chunking (we render a few blocks at a time)
	if(aggregate_counter_ == CHUNKING) {
		// Do initialization and reset stuff
		aggregate_counter_ = 0;
		block_counter = 0;
		uint interval, particle_index = 0;
		float v;
		
		// Then, make sure we don't schedule active grains
		vector<Particle *> inactive_particles = getInactiveParticles();
		uint max_particles = inactive_particles.size();
		
		// Calculate the minimum interval and number of particles (for deterministic approach)
		int minimum_interval = (int) (grain_size->value * (1 + (1.0 - density->value)));
		uint n_particles = (uint) floor((block_size) / minimum_interval) + 1;
		n_particles = n_particles > max_particles ? max_particles : n_particles;
		
		// Search algorithm and apply it
		switch (GranulationAlgorithm(round(algorithm->value * 3))) {
			case ga_Deterministic:
				// Iterate and trigger grains
				for (int i = 0; i < n_particles; i++) {
					inactive_particles[i]->start = i * minimum_interval;
					inactive_particles[i]->length = (uint) grain_size->value;
					inactive_particles[i]->in_queue_ = true;
				}
				break;
			case ga_DivideDown:
				while (max_particles > 1) {
					interval = block_size / max_particles;
					
					if (interval > minimum_interval) {
						for (uint i = 0; i < (block_size - interval); i += interval) {
							if (max_particles > 0) {
								v = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));;
								if (v < density->value) {
									inactive_particles[particle_index]->start = i;
									inactive_particles[particle_index]->length = grain_size->value;
									inactive_particles[particle_index]->in_queue_ = true;
									max_particles--;
									particle_index++;
								}
							}
						}
					} else {
						max_particles = 0;
					}
				}
				break;
			case ga_RandomStart:
				for (int i = 0; i < max_particles; i++) {
					v = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
					if (v < density->value && particle_index < MAX_NUM_PARTICLES) {
						inactive_particles[particle_index]->start = v * block_size;
						inactive_particles[particle_index]->in_queue_ = true;
						particle_index++;
					}
				}
				break;
			case ga_Note:
				minimum_interval = ((float) samplerate / (float) block_size) * frequency;
				n_particles = block_size / minimum_interval;
				for(int i = 0; i < n_particles; i++) {
					if(particle_index < MAX_NUM_PARTICLES) {
						inactive_particles[particle_index]->start = i * minimum_interval;
						inactive_particles[particle_index]->in_queue_ = true;
						particle_index++;
					}
				}
				break;
		}
		
		sort(particles.begin(), particles.begin(), [] (Particle *a, Particle *b) {
			return a->in_queue_ && (a->start < b->start);
		});
		
		for (auto &p: particles) {
			if(p->in_queue_) {
				p->prerender();
				next_.push(p);
			}
		}
		
		last_block_offset_ = minimum_interval - block_size;
		block_size = 0;
	} else {
		aggregate_counter_++;
	}
}

vector<Particle*> ParticleVoice::getInactiveParticles() {
	vector<Particle*> n;
	for(auto& p : particles) {
		if(!p->active) {
			n.push_back(p);
		}
	}
	return n;
}

void ParticleVoice::process () {
	long sample = 0;
	
	if(!active_particles.empty()) {
		for (auto &p: active_particles) {
			sample += p->getSample();
		}
		
		output->write((sample_t) ((float) sample / (float) active_particles.size()));
	} else {
		output->write(0);
	}
}

void ParticleVoice::tick() {
	for(auto& p : active_particles) {
		p->tick();
	}
	
	while(!next_.empty() && next_.front()->start <= block_counter) {
		active_particles.push_back(next_.front());
		next_.front()->active = true;
		next_.pop();
	}
	
	while(!active_particles.empty() && active_particles.front()->schedule_for_deletion_) {
		active_particles.front()->schedule_for_deletion_ = false;
		active_particles.front()->active = false;
		active_particles.pop_front();
	}
	
	block_counter++;
	output->tick();
}

void ParticleVoice::pitch (uint8_t key) {
	// Set speed AND LENGTH of all particles
	frequency = mtof(key, 430.0f);
	for(auto& p : particles) {
		p->speed = mtof(key, 430.0f) / engine->getBaseFrequency(); // TODO: calculate speed for any frequency
	}
}