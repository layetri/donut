//
// Created by Daniël Kamp on 10/11/2021.
//

#include "Header/NoteHandler.h"

NoteHandler::NoteHandler(vector<Voice*>* voices) {
  this->voices = voices;
  voices_upper = new KeyboardHalf;
  voices_lower = new KeyboardHalf;
  last_controlled = new vector<KeyboardHalf*>;
}

NoteHandler::~NoteHandler() {
	delete voices_upper;
	delete voices_lower;
	delete last_controlled;
}

void NoteHandler::noteOn(Note* note) {
	// Find a free voice or steal the least recently used voice
	if(!findNextFree(note)) {
	  	stealLeastRecent(note);
	} else {
		used_voices++;
	}
}

void NoteHandler::noteOff(Note* note) {
	for(auto& v : *voices) {
		if(v->releaseIfMatches(note->pitch)) {
			used_voices--;
			return;
		}
	}
}

void NoteHandler::set(ParameterID parameter, uint16_t value) {
	// Check if there are any held notes
	if(inUse() > 0) {
		bool set_lower = false;
		bool set_upper = false;
		last_controlled->clear();
		// Find the associated half for these notes
		for(auto& v : *voices) {
			if(!v->isAvailable()) {
				if(count(voices_lower->voices->begin(), voices_lower->voices->end(), v) && !set_lower) {
					for(auto& r : *voices_lower->voices) {
						r->set(parameter, value);
					}
					set_lower = true;
					last_controlled->push_back(voices_lower);
				} else if(count(voices_upper->voices->begin(), voices_upper->voices->end(), v) && !set_upper) {
					for(auto& r : *voices_upper->voices) {
						r->set(parameter, value);
					}
					set_upper = true;
					last_controlled->push_back(voices_upper);
				}
			}
		}
	} else {
		for(auto& half : *last_controlled) {
			for(auto& voice : *half->voices) {
				voice->set(parameter, value);
			}
		}
	}
}

bool NoteHandler::findNextFree(Note *note) {
	KeyboardHalf* half = getHalf(note);
	verbose(half->name);
	for(int i = 0; i < half->voices->size(); i++) {
		if(half->voices->at(half->rr_index)->assignIfAvailable(note)) {
			incrementVoiceIndex(half);
			return true;
		}
		incrementVoiceIndex(half);
	}
	return false;
}

void NoteHandler::incrementVoiceIndex(KeyboardHalf* half) {
	half->rr_index = (half->rr_index + 1 < half->voices->size()) * (half->rr_index + 1);
}

void NoteHandler::stealLeastRecent(Note* note) {
	clock_t s = clock();
	Voice* voice_to_steal;
	KeyboardHalf* half = getHalf(note);

	for(auto& v : *half->voices) {
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

/**
*   @brief Splits the note range in two.
*   @param uint_8t MIDI note number (0-127) to split the keyboard at.
*/
void NoteHandler::setSplit (uint8_t key) {
	this->split_key = key;
	
	intelliSplit();
	
	voices_lower->voices->clear();
	voices_upper->voices->clear();
	
	for(int i = 0; i < NUMBER_OF_VOICES; i++) {
		if(i < split_index) {
			voices_lower->voices->push_back(voices->at(i));
//			cout << "l" << i << " ";
		} else {
			voices_upper->voices->push_back(voices->at(i));
//			cout << "u" << i << " ";
		}
	}
	cout << endl;
}

/**
*   @brief Set the ratio between the number of voices assigned to both keyboard halves.
*   @param float MIDI CC value (0-127) for the split ratio.
*/
void NoteHandler::setSplitRatio(float split_ratio) {
	split_index = split_ratio * NUMBER_OF_VOICES;
	
	intelliSplit();
}

/**
*   @brief Abstraction to provide bounds for split functionality.
*/
void NoteHandler::intelliSplit() {
	split_index =
		  (split_index > 0 && split_index < NUMBER_OF_VOICES) * split_index +
		  (split_index == 0) * (split_key > 0) +
		  (split_index == NUMBER_OF_VOICES) * (NUMBER_OF_VOICES - (split_key < 127));
	cout << "split: " << split_index << endl;
}

KeyboardHalf* NoteHandler::getHalf(Note* note) {
	if(note->pitch >= split_key) {
		return voices_upper;
	} else {
		return voices_lower;
	}
}

uint8_t NoteHandler::inUse() {
	return used_voices;
}
