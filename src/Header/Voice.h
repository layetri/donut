#pragma once

#include "Buffer.h"
#include "WaveShaper.h"
#include "WaveTable.h"
#include "Tensions.h"

#include "Square.h"
#include "Note.h"
#include "FloatBuffer.h"
#include "AddAndDivide.h"

#include "ADSR.h"
#include "LFO.h"
#include "Modulator.h"
#include "ModMatrix.h"

#include "LowPassFilter.h"
#include "ParameterStore.h"
#include "Tables.h"

#include <curses.h>

#include <vector>
#define WS

using namespace std;

class Voice {
  public:
    Voice(Buffer* output, ParameterPool* params, ModMatrix* mm, Tables* tables, uint8_t v_id=0);
    ~Voice();

    void assign(Note* note);
    bool assignIfAvailable(Note* note);
	bool releaseIfMatches(int note);
	bool isAvailable();
    void process();
	void release();
	void tick();
	void set(ParameterID, int);
	
	Buffer* getBuffer();
	sample_t getSample();
	clock_t getTime() const;
	Note* getNote();
  private:
	vector<Modulator*> modulators;
	vector<Buffer*> lfo_buf;
	vector<Source*> sources;
	vector<Buffer*> buffers;
	ParameterPool* parameters;
	
	Parameter *filter_cutoff, *ws_fm_amount;
	
	LowPassFilter *lpf;
	
	Buffer *output, *mixbus;
	AddAndDivide *mixer;
	Note *midi_note;
	ADSR2 *envelope, *mod_envelope;
	Tables *tables;
	
	bool available;
	uint8_t voice_id;
	clock_t last_used;
	int8_t harmonics_knob = 0;
	uint8_t pitch = 0;
	
	float crossmod = 0.5;
	
	
	// TMP
	bool enableWaveTables = true;
	bool enableWaveShaper = false;
	bool enableSub = false;
};