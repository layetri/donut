//
// Created by Daniël Kamp on 12/04/2022.
//

#ifndef DONUT_GUI_H
#define DONUT_GUI_H

#include <Global.h>
#include <System/ParameterStore.h>
#include <System/ModMatrix.h>

#include <string>
#include <mutex>

#if defined(BUILD_GUI_NCURSES)
#include <curses.h>
#elif defined(BUILD_GUI_IMGUI)
#include "../../imgui/imgui.h"
#include "../../imgui/backends/imgui_impl_glfw.h"
#include "../../imgui/backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#endif

#include <iostream>
#include <deque>
#include <queue>
#include <vector>

using namespace std;

class GUI {
	public:
		GUI (ParameterPool* parameters, ModMatrix* mod, queue<Event*>* event_queue);
		~GUI ();
		
		void initgui();
		string input();
		void output(string, bool lb=true, int x=-1, int y=-1, int c=0);
		void process();
		void loop();
		int cleanup();
		void stereoPlot(float *left, float *right, uint size);
	
	private:
		ParameterPool* parameters;
		ModMatrix* mod;
//		CommandPool* commands;
		
		int row, col, winy, winx;
		string cmd;
		char cmd_stream[80];
	
	#ifdef BUILD_GUI_IMGUI
		ImGuiIO* io;
		GLFWwindow* window;
		ImVec4 clear_color = ImVec4(0.496f, 0.882f, 0.968f, 1.00f);
		
		bool demo_var = false;
		deque<string> log;
		uint log_size = 16;
		
		uint plot_size = 0;
		float *plotL = 0, *plotR = 0;
		
		queue<Event*>* event_queue;
		vector<Parameter*> mix_controls; // Store parameters that belong to Mixer
		vector<Parameter*> voice_controls;
	#endif
};


#endif //DONUT_GUI_H
