//
// Created by Daniël Kamp on 12/04/2022.
//

#ifndef DONUT_GUI_H
#define DONUT_GUI_H

#include <Global.h>
#include <System/ParameterStore.h>
#include <System/ModMatrix.h>
#include <ext/Fontaudio.h>

#include <unordered_map>
#include <string>
#include <mutex>

#if defined(BUILD_GUI_NCURSES)
#include <curses.h>
#elif defined(BUILD_GUI_IMGUI)
#include "../../imgui/imgui.h"
#include "../../imgui/backends/imgui_impl_glfw.h"
#include "../../imgui/backends/imgui_impl_opengl2.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#endif

#include <iostream>
#include <deque>
#include <queue>
#include <vector>

using namespace std;

enum WindowNames {
	win_Mixer,
	win_ModMatrix,
	win_Presets,
	win_Pads,
	win_MIDI_Console,
	win_MIDI_Devices,
	win_Oscilloscope,
};

enum PadMode {
	pm_MIDI,
	pm_Trigger
};

struct PresetGUIItem {
	string name;
	string created_at;
	string donut_version = "1.0";
};

struct ToggleWindowButton {
	string label = "";
	WindowNames name;
	bool status = false;
};

class GUI {
	public:
		GUI (ParameterPool* parameters, ModMatrix* mod, queue<Event*>* event_queue, bool* running);
		~GUI ();
		
		void initgui();
		string input();
		void output(string, bool lb=true, int x=-1, int y=-1, int c=0);
		void process();
		void loop();
		int cleanup();
		
		void stereoPlot(float *left, float *right, uint size);
		void updateMidiDevices(vector<string> inputs, vector<string> outputs);
		void updatePresets(vector<PresetGUIItem>);
	
	private:
		ParameterPool* parameters;
		ModMatrix* mod;
		bool* running;
//		CommandPool* commands;
		
		int row, col, winy, winx;
		string cmd;
		char cmd_stream[80];
	
	#ifdef BUILD_GUI_IMGUI
		ImGuiIO* io;
		GLFWwindow* window;
		ImVec4 clear_color = ImVec4(0.496f, 0.882f, 0.968f, 1.00f);
		ImFont *font_regular, *font_bold, *font_light, *text_small;
		ImGuiWindowFlags global_window_flags = 0;
		
		bool demo_var = false;
		deque<string> log;
		uint log_size = 16;
		
		uint plot_size = 0;
		float *plotL = 0, *plotR = 0;
		
		queue<Event*>* event_queue;
		
		vector<string> midi_inputs;
		vector<string> midi_outputs;
		uint16_t midi_in_selector = 0;
		uint16_t midi_out_selector = 0;
		
		vector<PresetGUIItem> presets;
		bool newPresetDialog = false;
		char pr_name[80] = "";
		uint selected_preset = 0;
		
		PadMode padMode = pm_MIDI;
		
		vector<ToggleWindowButton> mainButtons;
		vector<Parameter*> mix_controls; // Store parameters that belong to Mixer
		vector<Parameter*> voice_controls;
		unordered_map<string, string> parameterCategories;
	#endif
};


#endif //DONUT_GUI_H
