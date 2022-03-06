//
// Created by Daniël Kamp on 02/06/2021.
//

#ifndef KARPLUSSTRONG_PRESET_H
#define KARPLUSSTRONG_PRESET_H

#include "Global.h"
#include "ParameterStore.h"
#include <filesystem>
#include <fstream>
#include <sstream>

#include <string>
#include <map>
#include <vector>

#include <curses.h>

#define NUM_PRESETS 8
#if !defined(PLATFORM_DARWIN_X86)

#elif defined(PLATFORM_DARWIN_X86)
  #include <cmath>
  #include <string>
  #include <sstream>
#include <utility>
#endif

using namespace std;

struct Preset {
    map<string, float> parameters;
	string name = "default";
};

class PresetEngine {
	public:
		PresetEngine(ParameterPool*);
		~PresetEngine();

		void load(uint n);
		void store(string name);
		void log();

	private:
		ParameterPool* pool;
		vector<Preset*>* presets;
		vector<string>* available_presets;
		uint16_t selected;
};

#endif //KARPLUSSTRONG_PRESET_H
