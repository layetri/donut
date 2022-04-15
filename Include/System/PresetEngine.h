//
// Created by Daniël Kamp on 02/06/2021.
//

#ifndef DONUT_PRESET_H
#define DONUT_PRESET_H

#include <Global.h>
#include <System/ParameterStore.h>
#include <System/ModMatrix.h>
#include <System/SampleLibrary.h>
#include <System/GUI.h>

#include <Source/Sampler.h>
#include <ext/json.h>

#include <filesystem>
#include <fstream>
#include <sstream>

#include <string>
#include <tuple>
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
    vector<ParameterPreset*> parameters;
	string name = "default";
};

class PresetEngine {
	public:
		PresetEngine(ParameterPool*, ModMatrix*, Sampler*, SampleLibrary*, GUI*);
		~PresetEngine();

		void load(string name);
		void store(string name);
		void autosave();
		void log();

	private:
		ParameterPool* pool;
		ModMatrix* modmatrix;
		Sampler* sampler;
		SampleLibrary* library;
		GUI* gui;
		
		vector<Preset*>* presets;
		vector<string>* available_presets;
		uint16_t selected;
};

#endif //DONUT_PRESET_H
