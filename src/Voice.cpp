#include "Header/Voice.h"

Voice::Voice(Buffer* output, ParameterPool* params, Tables* tables, int v_id) {
	this->parameters = params;
	// Initialize buffers

	for(int i = 0; i < 4; i++) {
		osc_buf.push_back(new Buffer(500));
	}
	this->mixbus = new Buffer(500);
	this->output = output;

	// Initialize oscillators
	osc_wav.push_back(new WaveShaper(440.0, 0.0, osc_buf[s_WS1]));
	osc_wav.push_back(new WaveShaper(441, 0.0, osc_buf[s_WS2]));
	wt = new WaveTableOscillator(tables, osc_buf[s_WT], params);
	sub = new Square(220.0, 0.0, osc_buf[s_Sub]);

	for(int i = 0; i < 3; i++) {
		lfo_buf.push_back(new Buffer(50));
		lfo.push_back(new Sine(0, 0.0, lfo_buf[i]));
	}

	// Initialize envelope
	this->envelope = new ADSR2(2200, 2200, 0.7, 44100);
	this->envelope->regen();

	// Initialize voice mixer
	mixer = new AddAndDivide(&osc_buf, 4, mixbus);
	// Initialize voice filter
	this->lpf = new LowPassFilter(16000.0, mixbus, output);

	// Do various setup things
	voice_id = v_id;
	this->tables = tables;
	last_used = clock();
	available = true;
}

Voice::~Voice() {
	for(auto& s : osc_wav) {
		delete s;
	}
	for(auto& l : lfo) {
		delete l;
	}
	delete mixer;
	delete sub;
	delete lpf;
}

void Voice::process() {
	for(auto& l : lfo) {
		l->process();
	}
	// Process the second oscillator
	// TODO: turn different oscillators on and off
	if(enableWaveShaper) {
		osc_wav[1]->process();

		// Do FM modulation
		osc_wav[0]->setFrequency((osc_wav[1]->mtof(pitch + 12) *
			abs(osc_buf[1]->getCurrentSample() / (float) (2 * SAMPLE_MAX_VALUE)) *
			parameters->value(p_FM_Amount)) + osc_wav[1]->mtof(pitch));

		osc_wav[0]->process();
	}
	if(enableSub) {
		sub->process();
	}

	wt->process();

	// Apply amplitude envelope
	mixer->setMultiplier(envelope->getMultiplier());
	mixer->process();
	lpf->process();
}

void Voice::tick() {
	lpf->tick();
	wt->tick();

	output->tick();
	output->flush();
	for(auto& b : osc_buf) {
		b->tick();
	}
	for(auto& b : lfo_buf) {
		b->tick();
	}
	envelope->step();
}

void Voice::assign(Note* note) {
	this->midi_note = note;
	this->pitch = note->pitch;

	// Set WaveShaper pitch
	for (auto &ws: osc_wav) {
		ws->pitch(note->pitch);
	}

	sub->pitch(note->pitch);

	// Set WaveTable pitch
	wt->pitch(note->pitch);

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

void Voice::set(ParameterID parameter, int value) {
	switch(parameter) {
		case p_Harmonics:
			harmonics_knob = ((value + 1) / 4) - 16;
			parameters->set(p_Harmonics, harmonics_knob);
			for(auto& o : osc_wav) {
				o->setHarmonics(harmonics_knob);
			}
			break;
		case p_Detune:
			osc_wav[1]->setBaseFrequency((value / parameters->value(p_Detune_Range)) + 440.0);
			break;
		case p_Detune_Range:
			parameters->set(p_Detune_Range, 1.0f - (127.0f / value));
			break;
		case p_Filter_Cutoff:
			lpf->setFrequency((value / 127.0f) * 16000.0);
			parameters->set(p_Filter_Cutoff, (value / 127.0f) * 16000.0);
			break;
		case p_Filter_Resonance:
			lpf->setResonance(0.5f + (value / 12.7f));
			parameters->set(p_Filter_Resonance, 0.5f + (value / 12.7f));
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
		case p_LFO1_Rate:
			lfo[0]->setFrequency((value / 127.0f) * 220.0);
			break;
		case p_FM_Amount:
			parameters->set(p_FM_Amount, value / 127.0f);
			break;
		case p_FM_KeyTrack:
			parameters->set(p_FM_KeyTrack, value / 127.0f);
			break;
		case p_WS_Toggle:
			enableWaveShaper = !enableWaveShaper;
			mixer->setChannels(mixer->getChannels() + ((enableWaveShaper * 2 - 1) * 2));
			osc_buf[s_WS1]->flush();
			osc_buf[s_WS2]->flush();
			break;
		case p_WT_Toggle:
			enableWaveTables = !enableWaveTables;
			mixer->setChannels(mixer->getChannels() + (enableWaveTables * 2 - 1));
			osc_buf[s_WT]->flush();
			break;
		case p_Sub_Toggle:
			enableSub = !enableSub;
			mixer->setChannels(mixer->getChannels() + (enableSub * 2 - 1));
			osc_buf[s_Sub]->flush();
			break;
		case p_WT_Shape:
			parameters->set(p_WT_Shape, value / 63.5f);
			wt->setMix(value / 63.5f);
			break;
		default:
			cout << "Parameter doesn't exist" << endl;
			break;
	}
}

bool Voice::isAvailable() {
	return available;
}