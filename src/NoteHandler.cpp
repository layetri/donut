//
// Created by Daniël Kamp on 10/11/2021.
//

#include <System/NoteHandler.h>

NoteHandler::NoteHandler(vector<unique_ptr<Voice>>& voices) : voices(voices) {
  voices_upper = make_unique<KeyboardHalf>();
  voices_lower = make_unique<KeyboardHalf>();
  last_controlled.push_back(voices_upper.get());
  setSplit(0);
}

NoteHandler::~NoteHandler() {
}

void NoteHandler::noteOn(Note note) {
	// Find a free voice or steal the least recently used voice
	if(!findNextFree(note)) {
	  	stealLeastRecent(note);
	} else {
		used_voices++;
	}
}

void NoteHandler::noteOff(Note note) {
	for(auto& v : voices) {
		if(v->releaseIfMatches(note.pitch)) {
			used_voices--;
			return;
		}
	}
}

void NoteHandler::set(ParameterID parameter, uint16_t value) {
	const auto isVoiceInUse = [](auto& voice) { return !voice->isAvailable(); };
	
	const auto isThereAnActiveVoiceInHalf = [&] (auto& half) {
    			return count_if(half->voices.begin(),
    							half->voices.end(),
    							[&] (auto& voice) { return isVoiceInUse(voice); }) > 0;
	};
	
	if(inUse() > 0) {
		last_controlled.clear();
		
		if (isThereAnActiveVoiceInHalf(voices_lower)) {
			for(auto& r : voices_lower->voices) {
				r->set(parameter, value);
			}
			last_controlled.push_back(voices_lower.get());
		}
		
		if (isThereAnActiveVoiceInHalf(voices_upper)) {
			for(auto& r : voices_upper->voices) {
				r->set(parameter, value);
			}
			last_controlled.push_back(voices_upper.get());
		}
	} else {
		for(auto& half : last_controlled) {
			for(auto& voice : half->voices) {
				voice->set(parameter, value);
			}
		}
	}
}

bool NoteHandler::findNextFree(Note note) {
	KeyboardHalf& half = getHalf(note);
	for(int i = 0; i < half.voices.size(); i++) {
		if(half.voices.at(half.rr_index)->assignIfAvailable(note)) {
			incrementVoiceIndex(half);
			return true;
		}
		incrementVoiceIndex(half);
	}
	return false;
}

void NoteHandler::incrementVoiceIndex(KeyboardHalf& half) {
	half.rr_index = (half.rr_index + 1 < half.voices.size()) * (half.rr_index + 1);
}

void NoteHandler::stealLeastRecent(Note note) {
	clock_t s = clock();
	Voice* voice_to_steal = nullptr;
	KeyboardHalf& half = getHalf(note);

	for(auto& v : half.voices) {
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
    Note tmp = note_queue.front();
    note_queue.pop();

    for (auto &voice : voices) {
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
	
	voices_lower->voices.clear();
	voices_upper->voices.clear();
	
	for(int i = 0; i < NUMBER_OF_VOICES; i++) {
		if(i < split_index) {
			voices_lower->voices.push_back(voices.at(i).get());
//			cout << "l" << i << " ";
		} else {
			voices_upper->voices.push_back(voices.at(i).get());
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
	split_index = (split_key / 127.0) * (NUMBER_OF_VOICES - 1);
	split_index =
		  (split_index > 0 && split_index < NUMBER_OF_VOICES) * split_index +
		  (split_index == 0) * (split_key > 0) +
		  (split_index == NUMBER_OF_VOICES) * (NUMBER_OF_VOICES - (split_key < 127));
}

KeyboardHalf& NoteHandler::getHalf(Note note) {
	if(note.pitch >= split_key) {
		return *voices_upper;
	} else {
		return *voices_lower;
	}
}

uint8_t NoteHandler::inUse() {
	return used_voices;
}
