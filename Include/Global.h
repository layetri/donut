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
  #include <utility>
#include <vector>
  #include <string>
  #include <cmath>
#endif

// Add global likely/unlikely directives
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

// Provide a global sample template type
typedef int16_t sample_t;
#define SAMPLE_MAX_VALUE 32768
#define SAMPLE_MIN_VALUE -32767
#define PARAMETER_MAX_VALUE 65535

// Provide a global samplerate value;
extern unsigned int samplerate;

// Project-specific global code goes here!
#define NUM_INPUTS 1
#define NUMBER_OF_VOICES 12
#define MAX_ENVELOPE_LENGTH 220500
#define TABLE_FREQUENCY 1.0

typedef std::vector<unsigned char> midi_message_t;

enum SourceID {
	s_WS1,
	s_WS2,
	s_WT1,
	s_WT2,
	s_KS,
	s_Sampler,
	s_Particles,
	s_Global
};

enum ModID {
	m_LFO1,
	m_LFO2,
	m_ADSR1,
	m_ADSR2,
	m_RND1,
	m_RND2
};

enum ParameterID {
	p_ADSR1_Attack,
	p_ADSR1_Decay,
	p_ADSR1_Sustain,
	p_ADSR1_Release,
	
	p_FM_Amount,
	p_FM_KeyTrack,
	
	p_LFO1_Rate,
	p_LFO1_Sync,
	
	p_LFO2_Rate,
	p_LFO2_Sync,
	
	p_RND1_Rate,
	p_RND1_Range,
	p_RND1_Sync,
	p_RND1_Slew,
	
	p_Filter_Cutoff,
	p_Filter_Resonance,
	p_Filter_KeyTrack,
	p_Filter_Type,
	
	p_ADSR2_Attack,
	p_ADSR2_Decay,
	p_ADSR2_Sustain,
	p_ADSR2_Release,
	
	p_MIDI_List,
	p_MIDI_In,
	p_MIDI_Out,
	
	p_WT1_Amount,
	p_WT1_Shape,
	p_WT1_BaseFrequency,
	p_WT1_Detune,
	p_WT1_Transpose,
	
	p_WT2_Amount,
	p_WT2_Shape,
	p_WT2_BaseFrequency,
	p_WT2_Detune,
	p_WT2_Transpose,
	
	p_WS1_Amount,
	p_WS1_BaseFrequency,
	p_WS1_Harmonics,
	p_WS1_Detune,
	p_WS1_Detune_Range,
	p_WS1_Transpose,
	
	p_WS2_Amount,
	p_WS2_BaseFrequency,
	p_WS2_Harmonics,
	p_WS2_Detune,
	p_WS2_Detune_Range,
	p_WS2_Transpose,
	
	p_KS_Amount,
	p_KS_DelayTime,
	p_KS_Feedback,
	p_KS_FilterCutoff,
	p_KS_Transpose,
	
	p_Sampler_Amount,
	p_Sampler_Start,
	p_Sampler_Length,
	p_Sampler_Speed,
	p_Sampler_Direction,
	p_Sampler_Transpose,
	
	p_Particles_Amount,
	p_Particles_Shape,
	p_Particles_Density,
	p_Particles_Algorithm,
	p_Particles_GrainSize,
	p_Particles_Position,
	
	p_KeyboardSplit,
	
	p_FX_Delay_Amount,
	p_FX_Delay_DTLeft,
	p_FX_Delay_DTRight,
	p_FX_Delay_FBLeft,
	p_FX_Delay_FBRight,
	
	p_FX_Fold_Amount,
	p_FX_Fold_Gain,
	
	p_FX_Glitch_Amount,
	p_FX_Glitch_Stutter,
	p_FX_Glitch_Reverse,
	p_FX_Glitch_Repeat,
	p_FX_Glitch_Sync,
	
	p_OutputHPF_Frequency,
	
	p_VoiceMaster,
	p_Master,
	p_BPM,
	p_Generic,
	p_NotFound,
	p_Exit
};

enum ControlID {
	c_Generic,
	c_NoteOn,
	c_NoteOff,
	c_ccChange,
	c_RemapOn,
	c_PresetsList,
	c_PresetLoad,
	c_PresetStore,
	c_SampleList,
	c_SampleLoad,
	c_SampleLoaded,
	c_SamplerAddRegion,
	c_SamplerSetRoot,
	c_ControlsStore,
	c_ControlsLoad,
	c_Sustain,
	c_Transpose,
	c_Split
};

enum EventType {
	e_Midi,
	e_Control,
	e_Feedback,
	e_System,
	e_Transpose
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
	SourceID source = s_Global;
	std::string str_content;
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
	ControlEvent(ControlID cid, uint16_t value=0) : Event {e_Control} {
		this->cid = cid;
		this->value = value;
	};
};

struct StringEvent : public Event {
	StringEvent(ControlID cid, std::string str, uint16_t value=0) : Event {e_Control} {
		this->str_content = std::move(str);
		this->cid = cid;
		this->value = value;
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
