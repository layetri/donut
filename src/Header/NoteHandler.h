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

using namespace std;

class NoteHandler {
  public:
    NoteHandler(vector<Voice*>* voices);
    ~NoteHandler();

    void noteOn(Note* note);
	void noteOff(Note* note);

	bool findNextFree(Note* note);
	void stealLeastRecent(Note* note);
	void incrementVoiceIndex();
    void tick();

  private:
    vector<Voice*>* voices;
    queue<Note*> note_queue;

	int voice_index = 0;
};


#endif //DONUT_NOTEHANDLER_H
