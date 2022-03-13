#pragma once

#include "Buffer.h"
#include "Modulator.h"
#include "WaveShaper.h"
#include "WaveTable.h"
#include "Sine.h"
#include "Square.h"
#include "Note.h"
#include "FloatBuffer.h"
#include "AddAndDivide.h"
#include "ADSR.h"
#include "LowPassFilter.h"
#include "ParameterStore.h"
#include "Tables.h"

#include <vector>

using namespace std;

class Voice {
  public:
    Voice(Buffer* output, ParameterPool* params, Tables* tables, int v_id=0);
    ~Voice();

    void assign(Note* note);
    bool assignIfAvailable(Note* note);
	bool releaseIfMatches(int note);
	bool isAvailable();
    void process();
	void release();
	void tick();
	sample_t getSample();

	void set(ParameterID, int);

	[[nodiscard]] clock_t getTime() const;

  private:
    vector<Modulator*> mods;
    vector<WaveShaper*> osc_wav;
    vector<Buffer*> osc_buf;
	vector<Sine*> lfo;
	vector<Buffer*> lfo_buf;
	Square* sub;
	WaveTableOscillator* wt;

	ParameterPool* parameters;

	LowPassFilter* lpf;
    Buffer* output;
	Buffer* mixbus;
    AddAndDivide* mixer;
	Note* midi_note;
	ADSR2* envelope;

	float crossmod = 0.5;
    bool available;
	clock_t last_used;
	int voice_id;
	short unsigned int pitch = 0;
	Tables* tables;

	int8_t harmonics_knob = 0;


	// TMP
	bool enableWaveTables = true;
	bool enableWaveShaper = false;
	bool enableSub = false;
};