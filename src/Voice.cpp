#include "Header/Voice.h"

Voice::Voice(Buffer* output, int v_id) {
	// Initialize buffers
	for(int i = 0; i < 2; i++) {
		osc_buf.push_back(new Buffer(samplerate));
	}
	this->mixbus = new Buffer(samplerate);
	this->output = output;

	// Initialize oscillators
	osc_wav.push_back(new WaveShaper(440.0, 0.0, osc_buf[0]));
	osc_wav.push_back(new WaveShaper(441, 0.0, osc_buf[1]));

	// Initialize envelope
	this->envelope = new ADSR2(2200, 2200, 0.7, 44100);
	this->envelope->regen();

	// Initialize voice mixer
	mixer = new AddAndDivide(&osc_buf, 2, mixbus);

	// Initialize voice filter
	this->lpf = new LowPassFilter(7500.0, mixbus, output);

	// Do various setup things
	voice_id = v_id;
	last_used = clock();
	available = true;
	verbose("Initialized voice " + to_string(voice_id));
}

Voice::~Voice() {
	for(auto& s : osc_wav) {
		delete s;
	}
	delete mixer;
}

void Voice::process() {
	for(auto& o : osc_wav) {
		o->process();
	}
	mixer->setMultiplier(envelope->getMultiplier());
	mixer->process();
	lpf->process();
}

void Voice::tick() {
	lpf->tick();

	output->tick();
	output->flush();
	for(auto& b : osc_buf) {
		b->tick();
	}
	envelope->step();
}

void Voice::assign(Note* note) {
	cout << "Using voice " << voice_id << endl;
	this->midi_note = note;
	this->pitch = note->pitch;
	for(auto& sqr : osc_wav) {
		sqr->pitch(note->pitch);
	}

	// TODO: velocity to envelope
	envelope->start();

	// Housekeeping
	last_used = clock();
	available = false;
}

void Voice::release() {
	envelope->noteOff();
//	mixer->setMultiplier(0.0);
	available = true;
	cout << "release " << voice_id << ", pitch: " << this->pitch << endl;
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

void Voice::set(ParameterStore parameter, int value) {
	switch(parameter) {
		case p_Harmonics:
			for(auto& o : osc_wav) {
				o->setHarmonics(((value + 1) / 4) - 16);
				cout << "Harmonics: " << ((value + 1) / 4) - 16 << endl;
			}
			break;
		case p_Detune:
			for(auto& o : osc_wav) {
//				o->setDetune(value);
			}
			break;
		case p_Cutoff:
			lpf->setFrequency((value / 127.0f) * 16000.0);
			break;
		case p_Attack:
			envelope->setAttack(((value / 127.0f) * 220500));
			cout << "att" << endl;
			break;
		case p_Decay:
			envelope->setDecay(((value / 127.0f) * 220500));
			break;
		case p_Sustain:
			envelope->setSustain((value / 127.0f));
			break;
		case p_Release:
			envelope->setRelease(((value / 127.0f) * 220500));
			break;
		default:
			cout << "Parameter doesn't exist" << endl;
			break;
	}
}