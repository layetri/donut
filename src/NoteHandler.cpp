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

/*!
 * @brief Distribute Note On messages
 * @param note
 */
void NoteHandler::noteOn(Note note) {
	// Find a free voice or steal the least recently used voice
	if(!findNextFree(note)) {
	  	stealLeastRecent(note);
	} else {
		used_voices++;
	}
}

/*!
 * @brief Distribute Note Off messages
 * @param note
 */
void NoteHandler::noteOff(Note note) {
	for(auto& v : voices) {
		if(v->releaseIfMatches(note.pitch)) {
			used_voices--;
			return;
		}
	}
}

/*!
 * @brief Sets a given parameter to a given value (int or float) on the most recently controlled voice group
 * @param parameter
 * @param value
 * @param float_value
 */
void NoteHandler::set(ParameterID parameter, int16_t value, float float_value) {
	const auto isVoiceInUse = [](auto& voice) { return !voice->isAvailable(); };
	
	const auto isThereAnActiveVoiceInHalf = [&] (auto& half) {
    			return count_if(half->voices.begin(),
    							half->voices.end(),
    							[&] (auto& voice) { return isVoiceInUse(voice); }) > 0;
	};
	
	// If any voices are currently being played, we need to figure out which voice(s) to assign values to
	if(inUse() > 0) {
		// Clear assignment memory
		last_controlled.clear();
		
		// For both keyboard halves, check if it contains any active voices
		if (isThereAnActiveVoiceInHalf(voices_lower)) {
			// If it does, assign the parameter value to all voices in this half
			for(auto& r : voices_lower->voices) {
				r->set(parameter, value, float_value);
			}
			// Remember that this was the last controlled half, for when no notes are being played
			last_controlled.push_back(voices_lower.get());
		}
		// Do this for the upper half of the keyboard too
		if (isThereAnActiveVoiceInHalf(voices_upper)) {
			for(auto& r : voices_upper->voices) {
				r->set(parameter, value, float_value);
			}
			last_controlled.push_back(voices_upper.get());
		}
	} else {
		// If no voices are currently playing, set the value on the voice group that was most recently changed
		// TODO: look into whether this is intuitive enough
		for(auto& half : last_controlled) {
			for(auto& voice : half->voices) {
				voice->set(parameter, value, float_value);
			}
		}
	}
}

/*!
 * @brief Assign a note to the next free voice, round-robin style
 * @param note
 * @return True if the note can be assigned, false if all voices are in use
 */
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

/*!
 * @brief Increment the voice index, round-robin style
 * @param half
 */
void NoteHandler::incrementVoiceIndex(KeyboardHalf& half) {
	half.rr_index = (half.rr_index + 1 < half.voices.size()) * (half.rr_index + 1);
}

/*!
 * @brief Assign a note to the least recently used voice
 * @param note
 */
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

/*!
 * @brief Set the MIDI note to split the keyboard at
 * @param key
 */
void NoteHandler::setSplit (uint8_t key) {
	this->split_key = key;
	intelliSplit();
	
	voices_lower->voices.clear();
	voices_upper->voices.clear();
	
	for(int i = 0; i < NUMBER_OF_VOICES; i++) {
		if(i < split_index) {
			voices_lower->voices.push_back(voices.at(i).get());
		} else {
			voices_upper->voices.push_back(voices.at(i).get());
		}
	}
	cout << endl;
}

// TODO: implement and document intelligent keyboard split
void NoteHandler::setSplitRatio(float split_ratio) {
	split_index = split_ratio * NUMBER_OF_VOICES;
	
	intelliSplit();
}

void NoteHandler::intelliSplit() {
	split_index = (split_key / 127.0) * (NUMBER_OF_VOICES - 1);
	split_index =
		  (split_index > 0 && split_index < NUMBER_OF_VOICES) * split_index +
		  (split_index == 0) * (split_key > 0) +
		  (split_index == NUMBER_OF_VOICES) * (NUMBER_OF_VOICES - (split_key < 127));
}

/*!
 * @brief Determine the half that contains a given note
 * @param note
 * @return The keyboard half that contains the given note
 */
KeyboardHalf& NoteHandler::getHalf(Note note) {
	if(note.pitch >= split_key) {
		return *voices_upper;
	} else {
		return *voices_lower;
	}
}

/*!
 * @brief Return the number of voices that are currently in use
 * @return The number of voices currently in use [0, 12]
 */
uint8_t NoteHandler::inUse() {
	return used_voices;
}
