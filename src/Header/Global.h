//
// Created by Daniël Kamp on 30/05/2021.
//

#pragma once
#ifndef DONUT_GLOBAL_H
#define DONUT_GLOBAL_H

#define DEVMODE
// Set platform to Darwin x86 (macOS)
#define PLATFORM_DARWIN_X86

// Make sure that there's only one platform at once
#if !defined(PLATFORM_DARWIN_X86)
  // Set platform to Arduino
  //#define PLATFORM_ARDUINO_ARM

  // Set platform to Teensy 4.0 (IMXRT1062)
  #define PLATFORM_TEENSY40
  #define PLATFORM_EMBEDDED
#else
  #define ENGINE_JACK
  #include <cstdint>
  #include <vector>
  #include <string>
#endif

// Add global likely/unlikely directives
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

// Provide a global sample template type
typedef int16_t sample_t;
#define SAMPLE_MAX_VALUE 32768
#define PARAMETER_MAX_VALUE 65535

// Provide a global samplerate value;
extern unsigned int samplerate;

// Project-specific global code goes here!
#define NUM_INPUTS 1
#define NUMBER_OF_VOICES 12
#define MAX_ENVELOPE_LENGTH 220500

typedef std::vector<unsigned char> midi_message_t;

enum ApplicationState {
	app_Idle,
	app_Sequencer
};

enum ParameterID {
	p_Harmonics,
	p_AMP_Attack,
	p_AMP_Decay,
	p_AMP_Sustain,
	p_AMP_Release,
	p_Filter_Cutoff,
	p_FM_Amount,
	p_FM_KeyTrack,
	p_LFO1_Rate,
	p_Detune,
	p_Detune_Range,
	p_Filter_Resonance,
	p_Filter_KeyTrack,
	p_Filter_Type,
	p_MOD_Attack,
	p_MOD_Decay,
	p_MOD_Sustain,
	p_MOD_Release,
	p_MIDI_List,
	p_MIDI_In,
	p_NotFound,
	p_Exit
};

enum ControlID {
	c_Generic,
	c_NoteOn,
	c_NoteOff,
	c_ccChange,
	c_RemapOn,
	c_Sustain,
};

enum EventType {
	e_Midi,
	e_Control,
	e_Feedback,
	e_System
};

struct HelpItem {
	std::string command;
	std::string description;
};

struct Event {
	EventType type;
	uint16_t cc = 0;
	uint16_t value = 0;

	ControlID cid = c_Generic;
	std::vector<uint16_t> content;

	void write(uint16_t c) {
		content.push_back(c);
	}
};

struct MidiEvent : public Event {
	MidiEvent(uint16_t cc, uint16_t value) : Event {e_Midi} {
		this->cc = cc;
		this->value = value;
	};
};

struct ControlEvent : public Event {
	ControlEvent(ControlID cid) : Event {e_Control} {
		this->cid = cid;
	};
};

#include <complex>
#include <valarray>
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

// Global logging function
#ifdef DEVMODE
  #if defined(PLATFORM_DARWIN_X86)
    #include <iostream>
  #else
    #include <Arduino.h>
  #endif

  template <typename T>
  void verbose(const T& message) {
    #if defined(PLATFORM_DARWIN_X86)
      std::cout << message << std::endl;
    #else
      Serial.println(message);
    #endif
  }
#endif

#endif //DONUT_GLOBAL_H
