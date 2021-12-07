//
// Created by Daniël Kamp on 10/11/2021.
//

#include "Header/NoteHandler.h"

NoteHandler::NoteHandler(vector<Voice*>* voices) {
  this->voices = voices;
}

NoteHandler::~NoteHandler() {}

void NoteHandler::noteOn(Note* note) {
	// Push note onto queue and wait for the next tick to assign it
	if(!findNextFree(note)) {
	  	stealLeastRecent(note);
	}
}

void NoteHandler::noteOff(Note* note) {
	for(auto& v : *voices) {
		if(v->releaseIfMatches(note->pitch)) {
			return;
		}
	}
}

bool NoteHandler::findNextFree(Note *note) {
	for(int i = 0; i < NUMBER_OF_VOICES; i++) {
		if(voices->at(voice_index)->assignIfAvailable(note)) {
			incrementVoiceIndex();
			return true;
		}
		incrementVoiceIndex();
	}
	return false;
}

void NoteHandler::incrementVoiceIndex() {
	voice_index = (voice_index + 1 < NUMBER_OF_VOICES) * (voice_index + 1);
}

void NoteHandler::stealLeastRecent(Note* note) {
	clock_t s = clock();
	Voice* voice_to_steal;

	for(auto& v : *voices) {
		if(v->getTime() < s) {
			s = v->getTime();
			voice_to_steal = v;
		}
	}

	voice_to_steal->assign(note);
}

void NoteHandler::tick() {
  // Assign all notes in queue - if any are left, delete them
  while(!note_queue.empty()) {
    Note *tmp = note_queue.front();
    note_queue.pop();

    for (auto &voice : *voices) {
      if (voice->assignIfAvailable(tmp)) {
        break;
      }
    }
  }
}