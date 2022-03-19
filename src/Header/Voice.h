#pragma once

#include "Buffer.h"
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
#define WS

using namespace std;

class Voice {
  public:
    Voice(Buffer* output, ParameterPool* params, Tables* tables, uint8_t v_id=0);
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
	uint8_t voice_id;
  private:
    vector<Modulator*> mods;
	vector<Buffer*> lfo_buf;
	vector<Source*> sources;
	vector<Buffer*> buffers;

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
	
	short unsigned int pitch = 0;
	Tables* tables;

	int8_t harmonics_knob = 0;


	// TMP
	bool enableWaveTables = true;
	bool enableWaveShaper = false;
	bool enableSub = false;
};