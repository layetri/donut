#include <System/Voice.h>

Voice::Voice(Buffer* output, ParameterPool* params, ModMatrix* mm, Tables* tables, Sampler* sampler, Particles* particles, GUI* gui, uint8_t v_id) {
	// Do various setup things
	this->voice_id = v_id;
	this->last_used = clock();
	this->available = true;
	this->parameters = params;
	this->particles = particles;
	this->gui = gui;
	
	// Initialize buffers
	this->mixbus = new Buffer(500);
	this->output = output;
	
	// Initialize oscillators
	sources.push_back(new WaveShaper(params, params->get(p_WS1_Detune, v_id), params->get(p_WS1_Harmonics, v_id), params->get(p_WS1_Transpose, v_id), s_WS1, v_id)); // WS1
	sources.push_back(new WaveShaper(params, params->get(p_WS2_Detune, v_id), params->get(p_WS2_Harmonics, v_id), params->get(p_WS2_Transpose, v_id), s_WS2, v_id)); // WS2
	sources.push_back(new WaveTableOscillator(tables, params, params->get(p_WT1_Detune, v_id), params->get(p_WT1_Shape, v_id), params->get(p_WT1_Transpose, v_id), s_WT1, v_id)); // WT
	sources.push_back(new WaveTableOscillator(tables, params, params->get(p_WT2_Detune, v_id), params->get(p_WT2_Shape, v_id), params->get(p_WT2_Transpose, v_id), s_WT2, v_id)); // WT
	sources.push_back(new Tensions(tables, params, v_id));
	sources.push_back(new SamplerVoice(sampler, params, v_id));
//	sources.push_back(new ParticleVoice(particles, params, v_id));
	
	// Grab source buffers
	for(auto& s : sources) {
		buffers.push_back(s->getBuffer());
	}
	
	buffers[s_WS1]->attachMultiplier(params->get(p_WS1_Amount, v_id));
	buffers[s_WS2]->attachMultiplier(params->get(p_WS2_Amount, v_id));
	buffers[s_WT1]->attachMultiplier(params->get(p_WT1_Amount, v_id));
	buffers[s_WT2]->attachMultiplier(params->get(p_WT2_Amount, v_id));
	buffers[s_KS]->attachMultiplier(params->get(p_KS_Amount, v_id));
	buffers[s_Sampler]->attachMultiplier(params->get(p_Sampler_Amount, v_id));
//	buffers[s_Particles]->attachMultiplier(params->get(p_Particles_Amount, v_id));
	
	modulators.push_back(new LFO(params->get(p_LFO1_Rate, v_id), params->get(p_LFO1_Sync, v_id), tables, m_LFO1, "lfo1", v_id));
	modulators.push_back(new LFO(params->get(p_LFO2_Rate, v_id), params->get(p_LFO2_Sync, v_id), tables, m_LFO2, "lfo2", v_id));


	// Initialize envelope
	modulators.push_back(new ADSR2(
		  params->get(p_ADSR1_Attack, v_id),
		  params->get(p_ADSR1_Decay, v_id),
		  params->get(p_ADSR1_Sustain, v_id),
		  params->get(p_ADSR1_Release, v_id),
		  m_ADSR1, "adsr1", v_id));

	modulators.push_back(new ADSR2(
		  params->get(p_ADSR2_Attack, v_id),
		  params->get(p_ADSR2_Decay, v_id),
		  params->get(p_ADSR2_Sustain, v_id),
		  params->get(p_ADSR2_Release, v_id),
		  m_ADSR2, "adsr2", v_id));
	
	modulators.push_back(new RandomGenerator(
		  params->get(p_RND1_Rate, v_id),
		  params->get(p_RND1_Range, v_id),
		  params->get(p_RND1_Slew, v_id),
		  params->get(p_RND1_Sync, v_id),
		  m_RND1, "rnd1", v_id));

	for(auto& m : modulators) {
		mm->store(m);
	}

	mm->link(params->get(p_WS1_Harmonics, v_id), modulators[m_LFO1], v_id, 10);
//	mm->link(params->get(p_WT1_Shape, v_id), modulators[m_RND1], v_id, 1.0f);
	mm->link(params->get(p_VoiceMaster, v_id), modulators[m_ADSR1], v_id);
//	mm->link(params->get(p_Particles_Density, v_id), modulators[m_ADSR2], v_id);

	// Initialize voice mixer
	mixer = new AddAndDivide(&buffers, params, v_id, mixbus);
	
	// Initialize voice filter
	this->lpf = new LowPassFilter(parameters->get(p_Filter_Cutoff, voice_id), mixbus, output);
	
	ws_fm_amount = parameters->get(p_FM_Amount, voice_id);
	multiplier = parameters->get(p_VoiceMaster, voice_id);
}

Voice::~Voice() {
	for(auto& s : sources) {
		delete s;
	}
	delete mixer;
	delete lpf;
}

void Voice::process() {
	for(auto& m : modulators) {
		m->process();
	}
	
//	if (auto ws = dynamic_cast<WaveShaper*> (sources[s_WS1]))
//	{
//		ws->fm((float) buffers[s_WS2]->getCurrentSample() / (float) SAMPLE_MAX_VALUE, ws_fm_amount->value);
//	}
//	dynamic_cast<WaveShaper*> (sources[s_WS1])->fm((float) buffers[s_WS2]->getCurrentSample() / (float) SAMPLE_MAX_VALUE, ws_fm_amount->value);
//
//	std::tuple<WaveShaper, Square, Sine, Oscillator> sources;
//
//	forEachTupleItem(sources, [] (auto& source) {
//		source.process();
//	});
//
//  sources.get<3>().fm(...);
	
	// Do FM modulation
	sources[s_WS1]->fm((float) buffers[s_WS2]->getCurrentSample() / (float) SAMPLE_MAX_VALUE, ws_fm_amount->value);
	
	// Process sources
	for(auto& s : sources) {
		if(s->getBuffer()->getMultiplier() > 0.0) {
			s->process();
		}
	}
	
	// Apply amplitude envelope
	mixer->process();
	lpf->process();
}

void Voice::tick() {
	lpf->tick();
	
	for(auto& s : sources) {
		s->tick();
	}

	for(auto& m : modulators) {
		m->tick();
	}
	
	mixer->tick();
	output->tick();
	output->flush();
}

void Voice::assign(Note note) {
	this->midi_note = note;
	this->pitch = note.pitch;
	
	// Set source pitch
	for(auto& s : sources) {
		s->pitch(note.pitch);
	}
	
	// Sync modulators
	for(auto& m : modulators) {
		m->sync();
	}

	// Set modulator velocity
	modulators[m_ADSR1]->start(sqrt(note.velocity / 127.0f));
	modulators[m_ADSR2]->start(sqrt(note.velocity / 127.0f));
	parameters->set(p_OutputHPF_Frequency, voice_id, Source::mtof(pitch, 440.0f) + 1.0f);

	// Housekeeping
	last_used = clock();
	available = false;
}

void Voice::release() {
	modulators[m_ADSR1]->noteOff();
	modulators[m_ADSR2]->noteOff();
	available = true;
}

void Voice::block(size_t block_size) {
	for(auto& s : sources) {
		s->block(block_size);
	}
}

bool Voice::assignIfAvailable(Note note) {
	bool a = available;
	if(available) {
		assign(note);
	}
	return a;
}

bool Voice::releaseIfMatches(int note) {
	if(!this->available && note == this->pitch) {
		release();
		return true;
	}
	return false;
}

clock_t Voice::getTime() const {
	return last_used;
}

sample_t Voice::getSample() {
	return output->getCurrentSample();
}

Note Voice::getNote() {
	return midi_note;
}

void Voice::set(ParameterID parameter, int value) {
	auto p = parameters->get(parameter, voice_id);
	switch(parameter) {
		case p_WS1_Harmonics:
			parameters->set(p_WS1_Harmonics, voice_id, ((value + 1) / 4) - 16);
			break;
		case p_WS2_Harmonics:
			parameters->set(p_WS2_Harmonics, voice_id, ((value + 1) / 4) - 16);
			break;
		case p_WS1_Detune:
			parameters->set(p_WS1_Detune, voice_id,
				(value / parameters->value(p_WS1_Detune_Range, voice_id)) + 440.0);
			break;
		case p_WS2_Detune:
			parameters->set(p_WS2_Detune, voice_id,
				(value / parameters->value(p_WS2_Detune_Range, voice_id)) + 440.0);
			break;
		case p_WS1_Detune_Range:
			parameters->set(p_WS1_Detune_Range, voice_id, 1.0f - (127.0f / value));
			break;
		case p_WS2_Detune_Range:
			parameters->set(p_WS2_Detune_Range, voice_id, 1.0f - (127.0f / value));
			break;
		case p_Filter_Resonance:
			parameters->set(p_Filter_Resonance, voice_id, 0.5f + (value / 12.7f));
			break;
		case p_LFO1_Rate:
			parameters->set(p_LFO1_Rate, voice_id, ((value / 127.0f) * 20.0) + 0.1);
			break;
		case p_Sampler_Amount:
			parameters->set(p_Sampler_Amount, voice_id, value / 127.0f);
			break;
		case p_Particles_Amount:
			parameters->set(p_Particles_Amount, voice_id, value / 127.0f);
			break;
		case p_Particles_Algorithm:
			parameters->set(p_Particles_Algorithm, voice_id, value / 127.0f);
			break;
		case p_Particles_Shape:
			parameters->set(p_Particles_Shape, voice_id, value / 127.0f);
			break;
		case p_Particles_Density:
			parameters->set(p_Particles_Density, voice_id, value / 127.0f);
			break;
		case p_Particles_GrainSize:
			parameters->set(p_Particles_GrainSize, voice_id, (value / 127.0f) * 5000.0f + 100.0f);
			break;
		case p_Particles_Position:
			parameters->set(p_Particles_Position, voice_id, (value / 127.0f) * (particles->getMaxPosition() - parameters->get(p_Particles_GrainSize, voice_id)->value));
			break;
		case p_WT1_Shape:
			parameters->set(p_WT1_Shape, voice_id, value / 63.5f);
			sources[s_WT1]->refresh();
			break;
		case p_WT2_Shape:
			parameters->set(p_WT2_Shape, voice_id, value / 63.5f);
			sources[s_WT2]->refresh();
			break;
		default:
			parameters->set(parameter, voice_id, ((value / 127.0f) * (p->max - p->min)) + p->min);
			break;
	}
}

bool Voice::isAvailable() {
	return available;
}

Buffer* Voice::getBuffer () {
	return output;
}