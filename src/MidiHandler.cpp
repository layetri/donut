//
// Created by Daniël Kamp on 15/12/2021.
//

#include "Header/MidiHandler.h"

#ifdef USE_MIDI_HANDLER
float MidiHandler::prescaleCutoff(uint8_t value) {
	return (value / 127.0) * 16000;
}
#endif