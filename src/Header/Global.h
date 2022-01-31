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
//  #include "CommandPool.h"

//  CommandPool command_pool();
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

struct Event {
	unsigned short cc;
	unsigned short value;
	unsigned char type = 'n';
};

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
	p_NotFound,
	p_Exit
};

#include <complex>
#include <valarray>
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

// Global logging function
#ifdef DEVMODE
  #if defined(PLATFORM_DARWIN_X86)
    #include <string>
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
