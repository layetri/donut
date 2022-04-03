#include "Header/Voice.h"

Voice::Voice(Buffer* output, ParameterPool* params, ModMatrix* mm, Tables* tables, uint8_t v_id) {
	// Do various setup things
	this->voice_id = v_id;
	this->last_used = clock();
	this->available = true;
	this->parameters = params;
	
	// Initialize buffers
	this->mixbus = new Buffer(500);
	this->output = output;

	// Initialize oscillators
	sources.push_back(new WaveShaper(params, params->get(p_WS1_Detune, v_id), params->get(p_WS1_Harmonics, v_id), params->get(p_WS1_Transpose, v_id), s_WS1, v_id)); // WS1
	sources.push_back(new WaveShaper(params, params->get(p_WS1_Detune, v_id), params->get(p_WS1_Harmonics, v_id), params->get(p_WS2_Transpose, v_id), s_WS2, v_id)); // WS2
	sources.push_back(new WaveTableOscillator(tables, params, params->get(p_WT1_Detune, v_id), params->get(p_WT1_Shape, v_id), params->get(p_WT1_Transpose, v_id), s_WT1, v_id)); // WT
	sources.push_back(new WaveTableOscillator(tables, params, params->get(p_WT2_Detune, v_id), params->get(p_WT2_Shape, v_id), params->get(p_WT2_Transpose, v_id), s_WT2, v_id)); // WT
	sources.push_back(new Tensions(tables, params, v_id));

	// Grab source buffers
	for(auto& s : sources) {
		buffers.push_back(s->getBuffer());
	}
	
	buffers[s_WS1]->attachMultiplier(params->get(p_WS1_Amount, v_id));
	buffers[s_WS2]->attachMultiplier(params->get(p_WS2_Amount, v_id));
	buffers[s_WT1]->attachMultiplier(params->get(p_WT1_Amount, v_id));
	buffers[s_WT2]->attachMultiplier(params->get(p_WT2_Amount, v_id));
	buffers[s_KS]->attachMultiplier(params->get(p_KS_Amount, v_id));
	
	modulators.push_back(new LFO(params->get(p_LFO1_Rate, v_id), params->get(p_LFO1_Sync, v_id), tables, m_LFO1, "lfo1", v_id));
	modulators.push_back(new LFO(params->get(p_LFO1_Rate, v_id), params->get(p_LFO1_Sync, v_id), tables, m_LFO2, "lfo2", v_id));
	mm->store(modulators[m_LFO1]);
	mm->store(modulators[m_LFO2]);
	
//	mm->link(params->get(p_WS1_Harmonics, v_id), modulators[m_LFO1], v_id);
	
	
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
	
	mm->store(modulators[m_ADSR1]);
	mm->store(modulators[m_ADSR2]);
	
//	mm->link(params->get(p_WS1_Amount, v_id), modulators[m_ADSR1], v_id);
//	mm->link(params->get(p_WS2_Amount, v_id), modulators[m_ADSR1], v_id);
//	mm->link(params->get(p_WT1_Amount, v_id), modulators[m_ADSR1], v_id);
//	mm->link(params->get(p_WT2_Amount, v_id), modulators[m_ADSR1], v_id);
	mm->link(params->get(p_VoiceMaster, v_id), modulators[m_ADSR1], v_id);

	// Initialize voice mixer
	mixer = new AddAndDivide(&buffers, params, v_id, mixbus);
	
	// Initialize voice filter
	this->lpf = new LowPassFilter(parameters->get(p_Filter_Cutoff, voice_id), mixbus, output);
	
	filter_cutoff = parameters->get(p_Filter_Cutoff, voice_id);
	ws_fm_amount = parameters->get(p_FM_Amount, voice_id);
}

Voice::~Voice() {
	for(auto& s : sources) {
		delete s;
	}
	delete mixer;
	delete lpf;
}

void Voice::process() {
//	verbose("start process " + to_string(voice_id));
	for(auto& m : modulators) {
		m->process();
	}
	
	// Do FM modulation
	sources[s_WS1]->fm(buffers[s_WS2]->getCurrentSample() / (float) SAMPLE_MAX_VALUE, ws_fm_amount->value);
	
	// Process sources
	for(auto& s : sources) {
		s->process();
	}
//	verbose("check");
	
	// Apply amplitude envelope
	mixer->process();
	lpf->process();
//	verbose("end process " + to_string(voice_id));
}

void Voice::tick() {
	lpf->tick();
	
	for(auto& s : sources) {
		s->tick();
	}

	for(auto& m : modulators) {
		m->tick();
	}
	
	output->tick();
	output->flush();
}

void Voice::assign(Note* note) {
	this->midi_note = note;
	this->pitch = note->pitch;
	
	// Set WaveTable pitch
	for(auto& s : sources) {
		s->pitch(note->pitch);
	}
	
	for(auto& m : modulators) {
		m->sync();
	}

	// TODO: velocity to envelope
	modulators[m_ADSR1]->start(0.6 + (note->velocity / 127.0f) * 0.4);
	modulators[m_ADSR2]->start(0.6 + (note->velocity / 127.0f) * 0.4);

	// Housekeeping
	last_used = clock();
	available = false;
}

void Voice::release() {
	modulators[m_ADSR1]->noteOff();
	modulators[m_ADSR2]->noteOff();
	available = true;
}

bool Voice::assignIfAvailable(Note *note) {
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

Note* Voice::getNote() {
	return midi_note;
}

void Voice::set(ParameterID parameter, int value) {
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
		case p_Filter_Cutoff:
			parameters->set(p_Filter_Cutoff, voice_id, (value / 127.0f) * 16000.0);
			break;
		case p_Filter_Resonance:
			lpf->setResonance(0.5f + (value / 12.7f));
			parameters->set(p_Filter_Resonance, voice_id, 0.5f + (value / 12.7f));
			break;
		case p_ADSR1_Attack:
			parameters->set(p_ADSR1_Attack, voice_id, ((value / 127.0f) * 220500));
			break;
		case p_ADSR1_Decay:
			parameters->set(p_ADSR1_Decay, voice_id, ((value / 127.0f) * 220500));
			break;
		case p_ADSR1_Sustain:
			parameters->set(p_ADSR1_Sustain, voice_id, (value / 127.0f));
			break;
		case p_ADSR1_Release:
			parameters->set(p_ADSR1_Release, voice_id, ((value / 127.0f) * 220500));
			break;
//		case p_LFO1_Rate:
//			lfo[0]->setFrequency((value / 127.0f) * 220.0);
//			break;
		case p_FM_Amount:
			parameters->set(p_FM_Amount, voice_id, value / 127.0f);
			break;
		case p_FM_KeyTrack:
			parameters->set(p_FM_KeyTrack, voice_id, value / 127.0f);
			break;
		case p_WS1_Amount:
			parameters->set(p_WS1_Amount, voice_id, value / 127.0f);
			break;
		case p_WS2_Amount:
			parameters->set(p_WS2_Amount, voice_id, value / 127.0f);
			break;
		case p_WT1_Amount:
			parameters->set(p_WT1_Amount, voice_id, value / 127.0f);
			break;
		case p_WT2_Amount:
			parameters->set(p_WT2_Amount, voice_id, value / 127.0f);
			break;
		case p_KS_Amount:
			parameters->set(p_KS_Amount, voice_id, value / 127.0f);
			break;
		case p_WT1_Shape:
			parameters->set(p_WT1_Shape, voice_id, value / 63.5f);
			sources[s_WT1]->refresh();
			break;
		case p_WT2_Shape:
			parameters->set(p_WT2_Shape, voice_id, value / 63.5f);
			sources[s_WT2]->refresh();
			break;
		case p_WT1_Transpose:
			parameters->set(p_WT1_Transpose, voice_id, value - 64);
			break;
		case p_WT2_Transpose:
			parameters->set(p_WT2_Transpose, voice_id, value - 64);
			break;
		case p_WS1_Transpose:
			parameters->set(p_WS1_Transpose, voice_id, value - 64);
			break;
		case p_WS2_Transpose:
			parameters->set(p_WS2_Transpose, voice_id, value - 64);
			break;
		case p_KS_Transpose:
			parameters->set(p_KS_Transpose, voice_id, value - 64);
			break;
		default:
			printw("Parameter doesn't exist");
			refresh();
			break;
	}
}

bool Voice::isAvailable() {
	return available;
}

Buffer* Voice::getBuffer () {
	return output;
}