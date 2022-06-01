//
// Created by Daniël Kamp on 10/11/2021.
// NoteHandler provides note-related utilities like voice allocation and stealing, MIDI handlers, you name it and it's here. Not actually though, we don't have cookies :(
//

#ifndef DONUT_NOTEHANDLER_H
#define DONUT_NOTEHANDLER_H

#include "Voice.h"
#include "Note.h"

#include <vector>
#include <queue>

#define LOWER_HALF 0
#define UPPER_HALF 1

using namespace std;

struct KeyboardHalf {
	vector<Voice*>* voices;
	uint8_t rr_index = 0;
	string name;
	
	KeyboardHalf() {
		voices = new vector<Voice*>;
	}
};

class NoteHandler {
  public:
    NoteHandler(vector<Voice*>* voices);
    ~NoteHandler();

    void noteOn(Note* note);
	void noteOff(Note* note);
	void set(ParameterID, uint16_t);
	
	bool findNextFree(Note* note);
	void stealLeastRecent(Note* note);
	void incrementVoiceIndex(KeyboardHalf* half);
    void tick();
	uint8_t inUse();
	
	void setSplit(uint8_t key);
	void setSplitRatio(float split_ratio);
	void intelliSplit();
	KeyboardHalf* getHalf(Note* note);
	
  private:
	vector<Voice*>* voices;
	KeyboardHalf* voices_lower;
	KeyboardHalf* voices_upper;
	vector<KeyboardHalf*>* last_controlled;
	
    queue<Note> note_queue;
	uint8_t used_voices = 0;
	uint8_t split_key = 0;
	uint8_t split_index = 3;

	int voice_index = 0;
};


#endif //DONUT_NOTEHANDLER_H
