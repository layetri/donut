#pragma once

#include <Source/WaveShaper.h>
#include <Source/WaveTable.h>
#include <Source/Tensions.h>
#include <Source/Square.h>
#include <Source/Sampler.h>
#include <Source/Particles.h>

#include <DSP/Buffer.h>
#include <DSP/AddAndDivide.h>
#include <DSP/FloatBuffer.h>
#include <DSP/LowPassFilter.h>

#include <Modulator/ADSR.h>
#include <Modulator/LFO.h>
#include <Modulator/RandomGenerator.h>
#include <Modulator/Modulator.h>

#include <System/ModMatrix.h>
#include <System/Note.h>
#include <System/ParameterStore.h>
#include <System/Tables.h>
#include <System/GUI.h>

#include <curses.h>
#include <unistd.h>
#include <vector>
#define WS

using namespace std;

class Voice {
  public:
    Voice(Buffer* output, ParameterPool* params, ModMatrix* mm, Tables* tables, Sampler* sampler, Particles* particles, GUI* gui, uint8_t v_id=0);
    ~Voice();

    void assign(Note* note);
    bool assignIfAvailable(Note* note);
	bool releaseIfMatches(int note);
	bool isAvailable();
    void process();
	void release();
	void tick();
	void set(ParameterID, int);
	void block(size_t block_size);
	
	Buffer* getBuffer();
	sample_t getSample();
	clock_t getTime() const;
	Note getNote();
	
	Parameter* multiplier;
  private:
	vector<Modulator*> modulators;
	vector<Source*> sources;
	vector<Buffer*> buffers;
	ParameterPool* parameters;
	Particles* particles;
	
	Parameter *filter_cutoff, *ws_fm_amount;
	
	LowPassFilter *lpf;
	
	Buffer *output, *mixbus;
	AddAndDivide *mixer;
	Note midi_note;
	GUI *gui;
	
	bool available;
	uint8_t voice_id;
	clock_t last_used;
	uint8_t pitch = 0;
};