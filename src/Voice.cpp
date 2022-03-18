#include "Header/Voice.h"

Voice::Voice(Buffer* output, ParameterPool* params, Tables* tables, uint8_t v_id) {
	// Do various setup things
	this->voice_id = v_id;
	this->tables = tables;
	this->last_used = clock();
	this->available = true;
	this->parameters = params;
	
	// Initialize buffers
	this->mixbus = new Buffer(500);
	this->output = output;

	// Initialize oscillators
	sources.push_back(new WaveShaper(params, s_WS1, v_id)); // WS1
	sources.push_back(new WaveShaper(params, s_WS2, v_id)); // WS2
	sources.push_back(new WaveTableOscillator(tables, params, s_WT1, v_id)); // WT
	sources.push_back(new WaveTableOscillator(tables, params, s_WT2, v_id)); // WT
//	sources.push_back(new Square(220.0, params, v_id)); // Sub
	
	// Grab source buffers
	for(auto& s : sources) {
		buffers.push_back(s->getBuffer());
	}

//	modulators.push_back(new LFO());

	// Initialize envelope
	this->envelope = new ADSR2(2200, 2200, 0.7, 44100);
	this->envelope->regen();

	// Initialize voice mixer
	mixer = new AddAndDivide(&buffers, 4, mixbus);
	
	// Initialize voice filter
	this->lpf = new LowPassFilter(16000.0, mixbus, output);
}

Voice::~Voice() {
	for(auto& s : sources) {
		delete s;
	}
	delete mixer;
	delete lpf;
}

void Voice::process() {
	// Do FM modulation
	// FM takes 23 clock cycles
	// Rest takes ~50 cycles
	// Whole process takes 76 clock cycles
//	sources[s_WS1]->fm(buffers[s_WS2]->getCurrentSample() / (float) SAMPLE_MAX_VALUE);
	
	// Process sources
	for(auto& s : sources) {
		s->process();
	}
	
	// Apply amplitude envelope
	mixer->setMultiplier(envelope->getMultiplier());
	mixer->process();
	lpf->process();
}

void Voice::tick() {
	lpf->tick();
	
	for(auto& s : sources) {
		s->tick();
	}
	
	output->tick();
	output->flush();
	envelope->step();
}

void Voice::assign(Note* note) {
	this->midi_note = note;
	this->pitch = note->pitch;
	
	// Set WaveTable pitch
	for(auto& s : sources) {
		s->pitch(note->pitch);
	}

	// TODO: velocity to envelope
	envelope->start();

	// Housekeeping
	last_used = clock();
	available = false;
}

void Voice::release() {
	envelope->noteOff();
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
		case p_WS_Harmonics:
			parameters->set(p_WS_Harmonics, voice_id, ((value + 1) / 4) - 16);
			sources[s_WS1]->refresh();
			sources[s_WS2]->refresh();
			break;
		case p_WS_Detune:
			parameters->set(p_WS_Detune, voice_id,
				(value / parameters->value(p_WS_Detune_Range, voice_id)) + 440.0);
			sources[s_WS2]->refresh();
//			sources[s_WT2]->refresh();
			break;
		case p_WS_Detune_Range:
			parameters->set(p_WS_Detune_Range, voice_id, 1.0f - (127.0f / value));
			break;
		case p_Filter_Cutoff:
			lpf->setFrequency((value / 127.0f) * 16000.0);
			parameters->set(p_Filter_Cutoff, voice_id, (value / 127.0f) * 16000.0);
			break;
		case p_Filter_Resonance:
			lpf->setResonance(0.5f + (value / 12.7f));
			parameters->set(p_Filter_Resonance, voice_id, 0.5f + (value / 12.7f));
			break;
		case p_AMP_Attack:
			envelope->setAttack(((value / 127.0f) * 220500));
			break;
		case p_AMP_Decay:
			envelope->setDecay(((value / 127.0f) * 220500));
			break;
		case p_AMP_Sustain:
			envelope->setSustain((value / 127.0f));
			break;
		case p_AMP_Release:
			envelope->setRelease(((value / 127.0f) * 220500));
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
		case p_WS_Toggle:
			enableWaveShaper = !enableWaveShaper;
			mixer->setChannels(mixer->getChannels() - (enableWaveShaper * 2));
			buffers[s_WS1]->setMultiplier((float) (enableWaveShaper * 1.0));
			buffers[s_WS2]->setMultiplier((float) (enableWaveShaper * 1.0));
			break;
		case p_WT_Toggle:
			enableWaveTables = !enableWaveTables;
			mixer->setChannels(mixer->getChannels() - enableWaveTables);
			buffers[s_WT1]->setMultiplier((float) (enableWaveTables * 1.0));
			buffers[s_WT2]->setMultiplier((float) (enableWaveTables * 1.0));
			break;
		case p_Sub_Toggle:
			enableSub = !enableSub;
			mixer->setChannels(mixer->getChannels() - enableSub);
			buffers[s_Sub]->setMultiplier((float) (enableSub * 1.0));
			verbose(enableSub * 1.0);
			break;
		case p_WT_Shape:
			parameters->set(p_WT_Shape, voice_id, value / 63.5f);
			sources[s_WT1]->refresh();
			sources[s_WT2]->refresh();
			break;
		default:
			cout << "Parameter doesn't exist" << endl;
			break;
	}
}

bool Voice::isAvailable() {
	return available;
}