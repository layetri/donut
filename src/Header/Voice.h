#pragma once

#include "Buffer.h"
#include "Modulator.h"
#include "WaveShaper.h"
#include "Note.h"
#include "FloatBuffer.h"
#include "AddAndDivide.h"
#include "ADSR.h"
#include "LowPassFilter.h"

#include <vector>

using namespace std;

class Voice {
  public:
    Voice(Buffer* output, int v_id=0);
    ~Voice();

    void assign(Note* note);
    bool assignIfAvailable(Note* note);
	bool releaseIfMatches(int note);
    void process();
	void release();
	void tick();
	sample_t getSample();

	void set(ParameterStore, int);

	[[nodiscard]] clock_t getTime() const;

  private:
    vector<Modulator*> mods;
    vector<WaveShaper*> osc_wav;
    vector<Buffer*> osc_buf;

	LowPassFilter* lpf;
    Buffer* output;
	Buffer* mixbus;
    AddAndDivide* mixer;
	Note* midi_note;
	ADSR2* envelope;

    bool available;
	clock_t last_used;
	int voice_id;
	short unsigned int pitch = 0;
};