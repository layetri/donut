//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/main_darwin.h"
#include "Header/jack_module.h"
#include "Header/RtMidi.h"

#ifdef DEVMODE
  #include "Header/TestSynth.h"
#endif

void Darwin::ui() {
  while(true) {
//    CommandPool.namespace(ns).find(command).assign(arguments);
  }
}

void Darwin::main() {
  #ifdef ENGINE_JACK

  #endif

  Darwin::ui();
}

// Each module has
// - RegisterCommands
//
// CommandPool has all commands for all modules