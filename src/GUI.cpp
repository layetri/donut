//
// Created by Daniël Kamp on 12/04/2022.
//

#include "System/GUI.h"

mutex mtx2;

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GUI::GUI (ParameterPool* parameters, ModMatrix* mod, queue<Event*>* event_queue, DeveloperUtility* utils, bool* running) {
	this->parameters = parameters;
	this->event_queue = event_queue;
	this->mod = mod;
	this->running = running;
	this->devUtils = utils;
	
	#ifdef BUILD_GUI_IMGUI
	uint mtxsize = parameters->getAll()->size() * mod->getDict()->size();
	mtx_vals = new float[mtxsize];
	
	for(int i = 0; i < (sizeof(&mtx_vals) / sizeof(mtx_vals[0])); i++) {
		mtx_vals[i] = 0.0f;
	}
	#endif
	
	// Construct a GUI with ncurses, ImGui, or std::cout
	#if defined(BUILD_GUI_NCURSES)
		initscr();
		
		curs_set(0);
		use_default_colors();
		start_color();
		init_pair(0, COLOR_CYAN, -1);
		init_pair(1, COLOR_MAGENTA, -1);
		init_pair(2, COLOR_RED, -1);
		init_pair(3, COLOR_GREEN, -1);
		init_pair(4, COLOR_WHITE, -1);
		init_pair(5, COLOR_CYAN, -1);
	#elif defined(BUILD_GUI_IMGUI)
		mainButtons.push_back(ToggleWindowButton{ICON_FAD_HEADPHONES "Mixer", win_Mixer});
	#ifdef FEATURES_MODMATRIX
		mainButtons.push_back(ToggleWindowButton{ICON_FAD_MODULARPLUG "Mod Matrix", win_ModMatrix});
	#endif
		mainButtons.push_back(ToggleWindowButton{ICON_FAD_SAVE "Presets", win_Presets});
		mainButtons.push_back(ToggleWindowButton{ICON_FAD_WAVEFORM "Sampler", win_Sampler});
	#ifdef FEATURES_PADS
		mainButtons.push_back(ToggleWindowButton{ICON_FAD_DRUMPAD "Pads", win_Pads});
	#endif
		mainButtons.push_back(ToggleWindowButton{ICON_FAD_MIDIPLUG "MIDI Devices", win_MIDI_Devices});
		mainButtons.push_back(ToggleWindowButton{ICON_FAD_MODSINE "Oscilloscope", win_Oscilloscope});
	#ifdef DEVMODE
		mainButtons.push_back(ToggleWindowButton{ICON_FAD_CPU "DevTools", win_Devtools});
	#endif
		
		parameterCategories.insert({"adsr1", "ADSR1"});
		parameterCategories.insert({"adsr2", "ADSR2"});
		parameterCategories.insert({"lfo1", "LFO1"});
		parameterCategories.insert({"lfo2", "LFO2"});
		parameterCategories.insert({"rnd1", "Random 1"});
		parameterCategories.insert({"rnd2", "Random 2"});
		parameterCategories.insert({"filter", "Filter"});
		parameterCategories.insert({"wt1", "Basic OSC 1"});
		parameterCategories.insert({"wt2", "Basic OSC 2"});
		parameterCategories.insert({"ws1", "WaveShaper 1"});
		parameterCategories.insert({"ws2", "WaveShaper 2"});
		parameterCategories.insert({"ks", "Tensions"});
		parameterCategories.insert({"sampler", "Sampler"});
		parameterCategories.insert({"particles", "Particles"});
		parameterCategories.insert({"fxdelay", "StereoDelay"});
		
		for(auto& p : *parameters->getDictionary()) {
			if(p.key.find("amount") != string::npos) {
				mix_controls.push_back(parameters->get(p.pid, 0));
			} else {
				voice_controls.push_back(parameters->get(p.pid, 0));
			}
		}
		
		// Setup window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
			exit(1);
	
		// Create window with graphics context
	#ifdef SYS_LINUX
		// Set resolution for Pi touchscreen (generalizing to all Linux clients for now)
		window = glfwCreateWindow(800, 480, "DonutUI", NULL, NULL);
	#else
		// On non-Linux systems, create a 1280x720px window
		window = glfwCreateWindow(1280, 720, "Donut", NULL, NULL);
	#endif
		if (window == NULL)
			exit(1);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable vsync
		
		// Init ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		
		// Setup styles
		io = &ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		
		// Import fonts
		io->Fonts->AddFontFromFileTTF(".donut_runtime/fonts/NotoSans-Regular.ttf", 20.0f);
		
		static const ImWchar icons_ranges[] = { ICON_MIN_FAD, ICON_MAX_16_FAD, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.GlyphMinAdvanceX = 20.0f;
		icons_config.GlyphOffset = ImVec2(-2.0f, 5.0f);
		io->Fonts->AddFontFromFileTTF(".donut_runtime/fonts/fontaudio.ttf", 20.0f, &icons_config, icons_ranges );
		
		font_regular = io->Fonts->AddFontFromFileTTF(".donut_runtime/fonts/NotoSans-Regular.ttf", 17.0f);
		font_bold = io->Fonts->AddFontFromFileTTF(".donut_runtime/fonts/NotoSans-Bold.ttf", 20.0f);
		font_light = io->Fonts->AddFontFromFileTTF(".donut_runtime/fonts/NotoSans-Light.ttf", 20.0f);
		text_small = io->Fonts->AddFontFromFileTTF(".donut_runtime/fonts/NotoSans-Regular.ttf", 14.0f);
		text_small_bold = io->Fonts->AddFontFromFileTTF(".donut_runtime/fonts/NotoSans-Bold.ttf", 14.0f);
		
		io->Fonts->Build();
		
		// Set global window flags
		global_window_flags |= ImGuiWindowFlags_NoCollapse;
		global_window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
		io->ConfigWindowsMoveFromTitleBarOnly = true;
	
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL2_Init();
	#endif
}

GUI::~GUI () {
	// Destroy the GUI.
	#if defined(BUILD_GUI_NCURSES)
		cout << "\x1B[2J\x1B[H";
		curs_set(1);
	#elif defined(BUILD_GUI_IMGUI)
		delete mtx_vals;
		delete plotL;
		delete plotR;
	#endif
}

void GUI::initgui () {
	#if defined(BUILD_GUI_NCURSES)
		cout << "\x1B[2J\x1B[H";
		getmaxyx(stdscr, row, col);
		
		move(0, col-18);
		attron(COLOR_PAIR(3));
		printw("Donut");
		attroff(COLOR_PAIR(3));
		printw(" version 0.2");
		refresh();
		
		move(4,0);
		clrtoeol();
		attron(COLOR_PAIR(2));
		printw("Incoming MIDI:");
		attroff(COLOR_PAIR(2));
		refresh();
		
		move(8, 0);
		clrtoeol();
		printw("[");
		attron(COLOR_PAIR(3));
		printw(">");
		attroff(COLOR_PAIR(3));
		printw("] ");
		refresh();
		
		move(8, 4);
		clrtoeol();
		curs_set(1);
		refresh();
	#endif
}

string GUI::input() {
	// This function takes user input
	#if defined(BUILD_GUI_NCURSES)
		mtx2.lock();
		move(8, 4);
		clrtoeol();
		curs_set(1);
		refresh();
		mtx2.unlock();
		
		getstr(cmd_stream);
		cmd = cmd_stream;
	#elif defined(BUILD_GUI_COUT)
		getline(cin, cmd);
	#endif
	
	return cmd;
}

void GUI::output(const string line, bool lb, int x, int y, int c) {
	// This function displays a command's output on the screen
	#if defined(BUILD_GUI_NCURSES)
		curs_set(0);
		if(lb) {
			clrtobot();
			refresh();
		}
		
		if(x > -1 && y > -1) {
			getyx(stdscr,winy,winx);
			move(y, x);
			clrtoeol();
			attron(COLOR_PAIR(c));
			printw(line.c_str());
			attroff(COLOR_PAIR(c));
			move(winy, winx);
		} else {
			attron(COLOR_PAIR(c));
			printw(line.c_str());
			attroff(COLOR_PAIR(c));
			if(lb) {
				printw("\n");
			}
		}
		
		curs_set(1);
		refresh();
	#elif defined(BUILD_GUI_COUT)
		if(x > 0) {
			for(int i = 0; i < x; i++) {
				cout << " ";
			}
		}
		if(lb) {
			
			cout << line << endl;
		} else {
			cout << line;
		}
	#elif defined(BUILD_GUI_IMGUI)
		log.push_front(line);
		if(log.size() > log_size) {
			log.pop_back();
		}
	#endif
}

void GUI::loop() {
	#ifdef BUILD_GUI_IMGUI
	while(!glfwWindowShouldClose(window) && *running) {
		glfwPollEvents();
		
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		int fwx, fwy;
		glfwGetWindowSize(window, &fwx, &fwy);
		
		ImGuiStyle& window_style = ImGui::GetStyle();
		window_style.ChildRounding = 7.0f;
		window_style.FrameRounding = 7.0f;
		window_style.GrabRounding = 7.0f;
		window_style.TabRounding = 7.0f;
		window_style.PopupRounding = 7.0f;
		window_style.ScrollbarRounding = 7.0f;
		
		// Main window
		{
			ImGuiWindowFlags topbar_flags = 0;
			topbar_flags |= ImGuiWindowFlags_NoResize;
			topbar_flags |= ImGuiWindowFlags_NoMove;
			topbar_flags |= ImGuiWindowFlags_NoCollapse;
			topbar_flags |= ImGuiWindowFlags_NoTitleBar;
			
			ImGui::SetNextWindowSize(ImVec2(fwx, 45));
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::Begin("Application Controls", NULL, topbar_flags);
			for(auto& b : mainButtons) {
				ImGui::SameLine();
				
				ImGui::Button(b.label.c_str());
				if (ImGui::IsItemClicked()) {
					b.status = !b.status;
				}
			}
			
			ImGui::SameLine();
			if(ImGui::Button(ICON_FAD_POWERSWITCH)) {
				event_queue->push(new ControlEvent{c_SafeShutdown});
			}
			
			ImGui::End();
		}
		
		// MIDI Devices
		if(mainButtons[win_MIDI_Devices].status) {
			ImGui::Begin(ICON_FAD_MIDIPLUG "MIDI Devices");
			event_queue->push(new Event{e_System, 21 + p_MIDI_List, 0});
			
			if (ImGui::BeginListBox("MIDI Inputs")) {
				for (int n = 0; n < midi_inputs.size(); n++) {
					const bool is_selected = (midi_in_selector == n);
					if (ImGui::Selectable(midi_inputs[n].c_str(), is_selected)) {
						midi_in_selector = n;
						event_queue->push(new Event{e_System, 21 + p_MIDI_In, midi_in_selector});
					}
					
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
			
			ImGui::End();
		}
		
		// Output graph
		if(mainButtons[win_Oscilloscope].status) {
			ImGui::Begin("Oscilloscope");
			ImGui::PlotLines("Left", plotL, plot_size, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
			ImGui::PlotLines("Right", plotR, plot_size, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
			ImGui::End();
		}
		
		// Presets
		if(mainButtons[win_Presets].status) {
			ImGui::SetNextWindowSize(ImVec2(400, 327));
			ImGuiWindowFlags preset_window_flags = 0;
			preset_window_flags |= global_window_flags;
			preset_window_flags |= ImGuiWindowFlags_NoResize;
			ImGui::Begin("Presets", NULL, preset_window_flags);
			
			ImVec2 list_dims(120, 250);
			event_queue->push(new ControlEvent{c_PresetsList});
			
			if(ImGui::Button(ICON_FAD_SAVEAS "New..."))
				ImGui::OpenPopup(ICON_FAD_SAVEAS "Store preset");
			
			if(!presets.empty()) {
				ImGui::BeginGroup();
				if (ImGui::BeginListBox("##presets_list", list_dims)) {
					for (int n = 0; n < presets.size(); n++) {
						const bool is_selected = (selected_preset == n);
						if (ImGui::Selectable(presets[n].name.c_str(), is_selected)) {
							selected_preset = n;
						}
						
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndListBox();
				ImGui::EndGroup();
				ImGui::SameLine();
				
				ImGui::BeginGroup();
				ImGui::PushFont(font_bold);
				ImGui::Text("%s", presets[selected_preset].name.c_str());
				ImGui::PopFont();
				ImGui::PushFont(text_small);
				ImGui::Text("Modified: %s", presets[selected_preset].created_at.c_str());
				ImGui::PopFont();
				
				ImGui::SetCursorPos(ImVec2(140, ImGui::GetWindowHeight() - 40));
				if(ImGui::Button("Load preset")) {
					event_queue->push(new StringEvent{c_PresetLoad, presets[selected_preset].name});
				}
				ImGui::EndGroup();
			}
			
			if(ImGui::BeginPopupModal(ICON_FAD_SAVEAS "Store preset")) {
				ImGui::SetItemDefaultFocus();
				ImGui::InputText("Preset name", pr_name, 80);
				
				if(ImGui::Button("Store", ImVec2(120, 0))) {
					string pr_name_cast(pr_name);
					output(pr_name_cast);
					event_queue->push(new StringEvent{c_PresetStore, pr_name_cast});
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if(ImGui::Button("Cancel", ImVec2(120,0)))
					ImGui::CloseCurrentPopup();
				
				ImGui::EndPopup();
			}
			
			ImGui::End();
		}
		
		// Sampler
		if(mainButtons[win_Sampler].status) {
			ImGui::Begin("Sampler");
			ImGui::Text("Sampler implementation coming soon...");
			ImGui::End();
		}
		
	#ifdef FEATURES_PADS
		// Drumpads
		if(mainButtons[win_Pads].status) {
			ImGui::SetNextWindowSize(ImVec2(641, 380));
			ImGuiWindowFlags pads_flags = 0;
			pads_flags |= ImGuiWindowFlags_NoResize;
			pads_flags |= ImGuiWindowFlags_NoScrollbar;
			pads_flags |= ImGuiWindowFlags_NoCollapse;
			
			ImGui::Begin(ICON_FAD_DRUMPAD "Pads##pad_window", NULL, pads_flags);
			
			// TODO: add button mode controls
			if(ImGui::Button("MIDI")) {
				padMode = pm_MIDI;
			}
			ImGui::SameLine();
			if(ImGui::Button("Trigger")) {
				padMode = pm_Trigger;
			}
			
//			ImGui::Text("X: %f, Y: %f", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec2 button_sz(150, 150);
			int buttons_count = 8;
			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			for (int n = 0; n < buttons_count; n++) {
				ImGui::PushID(n);
				ImGui::Button(to_string(n).c_str(), button_sz);
				if(ImGui::IsItemActivated()) {
					switch(padMode) {
						case pm_MIDI:
							event_queue->push(new Event {e_NoteOn, (uint16_t) (padLowestNote + n), padVelocity});
							break;
						case pm_Trigger:
							
							break;
					}
				} else if(ImGui::IsItemDeactivated()) {
					switch(padMode) {
						case pm_MIDI:
							event_queue->push(new Event {e_NoteOff, (uint16_t) (padLowestNote + n), padVelocity});
							break;
						case pm_Trigger:
							
							break;
					}
				}
				
				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
				if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
					ImGui::SameLine();
				ImGui::PopID();
			}
			
			ImGui::End();
		}
	#endif
		
		// Voice controls
		{
			ImGui::SetNextWindowSize(ImVec2(270, fwy - 45));
			ImGui::SetNextWindowPos(ImVec2(fwx-270, 45));
			ImGui::Begin(ICON_FAD_SLIDERHANDLE_1 "Voice controls", NULL, global_window_flags);
			string category = "";
			int i = 0;
			for(auto& v : voice_controls) {
				auto name = parameters->translate(v->pid);
				if(name.find("_") != string::npos) {
					auto cat = name.substr(0, name.find_first_of("_"));
					
					if (category != cat) {
						category = cat;
						ImGui::Separator();
						ImGui::PushFont(font_bold);
						ImGui::Text("%s", parameterCategories.at(cat).c_str());
						ImGui::PopFont();
					}
					
					ImGui::PushFont(font_light);
					ImGui::Text("%s", name.substr(name.find_first_of("_") + 1).c_str());
					ImGui::PopFont();
					
					ImGui::PushItemWidth(240);
					ImGui::SliderFloat(("##slider" + to_string(i)).c_str(), &v->base_value, v->min, v->max, "%.5f");
					if (ImGui::IsItemEdited()) {
						event_queue->push(new Event{e_FloatControl, (uint16_t) (21 + v->pid), 0, v->base_value});
					}
					i++;
				}
			}
			ImGui::End();
		}
		
		// Modulation Matrix
		if(mainButtons[win_ModMatrix].status){
			ImGui::Begin("Mod Matrix");
			
			uint mtx_pos = 0;
			if (ImGui::BeginTable("modmtx", mod->getDict()->size() + 1)) {
				ImGui::TableNextRow();
				int col = 1;
				for (auto& m : *mod->getDict()) {
					ImGui::TableSetColumnIndex(col);
					ImGui::Text("%s", m.key.c_str());
					col++;
				}
				
				for (auto& p : *parameters->getDictionary()) {
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s", p.key.c_str());
					
					col = 1;
					for (auto& m : *mod->getDict()) {
						float v = 0.0f;
						ImGui::TableSetColumnIndex(col);
						ImGui::PushItemWidth(120);
						ImGui::SliderFloat(("##mod_" + to_string(mtx_pos)).c_str(), &mtx_vals[mtx_pos], 0.0f, 1.0f);
						if(ImGui::IsItemEdited()) {
							mod->setOrCreate(m.mid, p.pid, mtx_vals[mtx_pos]);
						}
						col++;
						mtx_pos++;
					}
				}
				ImGui::EndTable();
			}
			ImGui::End();
		}
		
		// Mixer
		if(mainButtons[win_Mixer].status){
			ImGui::SetNextWindowSize(ImVec2(0,0));
			ImGui::Begin("Mixer");
			for(int i = 0; i < mix_controls.size(); i++) {
				auto label = parameters->translate(mix_controls[i]->pid);
				ImGui::SameLine();
				
				ImGui::BeginGroup();
				ImGui::BeginChild(label.c_str(), ImVec2(80, 200), true);
				
				ImGui::PushFont(text_small_bold);
				label = label.substr(0, label.find_last_of("_"));
				auto windowWidth = ImGui::GetWindowSize().x;
				auto textWidth   = ImGui::CalcTextSize(parameterCategories.at(label).c_str()).x;
				ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
				ImGui::Text("%s", parameterCategories.at(label).c_str());
				ImGui::PopFont();
				
				ImGui::SetCursorPosX((windowWidth - 18) * 0.5f);
				ImGui::VSliderFloat("##mixer_slider", ImVec2(18, 160), &mix_controls[i]->value, 0.0f, 1.0f);
				if(ImGui::IsItemEdited()) {
					event_queue->push(new Event{e_Control, (uint16_t)(21 + mix_controls[i]->pid), (uint16_t) (127.0f * mix_controls[i]->value)});
				}
				ImGui::EndChild();
				ImGui::EndGroup();
			}
			ImGui::End();
		}
		
	#ifdef DEVMODE
		// Devtools
		if(mainButtons[win_Devtools].status) {
			ImGui::Begin("Developer tools");
			
			auto m = devUtils->getMeasurements();
			
			// Print samplerate
			ImGui::Text("Samplerate: ");
			ImGui::PushFont(font_bold);
			ImGui::SameLine();
			ImGui::Text("%u Hz", m.sampleRate);
			ImGui::PopFont();
			
			// Print block size
			ImGui::Text("Block size: ");
			ImGui::PushFont(font_bold);
			ImGui::SameLine();
			ImGui::Text("%u", m.blockSize);
			ImGui::PopFont();
			
			// Print processing times
			ImGui::Text("Average process time: ");
			ImGui::PushFont(font_bold);
			ImGui::SameLine();
			ImGui::Text("%u us", m.avgProcessTime);
			ImGui::PopFont();
			ImGui::SameLine();
			ImGui::PushFont(text_small);
			ImGui::Text(" (%u us max)", m.maxProcessTime);
			ImGui::PopFont();
			
			// Print JACK times
			ImGui::Text("Average JACK cycle time: ");
			ImGui::PushFont(font_bold);
			if(m.avgJackCycleTime > m.allowedCycleTime) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			}
			ImGui::SameLine();
			ImGui::Text("%u us", m.avgJackCycleTime);
			ImGui::PopFont();
			if(m.avgJackCycleTime > m.allowedCycleTime) {
				ImGui::PopStyleColor();
			}
			ImGui::SameLine();
			ImGui::PushFont(text_small);
			ImGui::Text(" (%u us allowed)", m.allowedCycleTime);
			ImGui::PopFont();
			
			ImGui::PlotLines("Process time", m.processTimes, 100, 0, NULL, 0.0f, 50.0f, ImVec2(0, 60));
			
			ImGui::BeginChild("App Monitor", ImVec2(0,0), true);
			ImGui::PushFont(font_regular);
			bool is_first = true;
			for(basic_string<char> line : log) {
				if(is_first){
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.443f, 0.909f, 0.96f, 1.0f));
				} else {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
				}
				
				ImGui::Text("%s", line.c_str());
				
				ImGui::PopStyleColor();
				if(is_first) {
					is_first = false;
				}
			}
			ImGui::PopFont();
			ImGui::EndChild();
			
			ImGui::End();
		}
	#endif
		
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(window);
	}
	#endif
}

int GUI::cleanup() {
	#ifdef BUILD_GUI_IMGUI
		// Cleanup
		ImGui_ImplOpenGL2_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		
		glfwDestroyWindow(window);
		glfwTerminate();
	#endif
	
	return 1;
}


void GUI::stereoPlot(float* left, float* right, uint size) {
#ifdef BUILD_GUI_IMGUI

	this->plotL = left;
	this->plotR = right;
	this->plot_size = size;
#endif
}

void GUI::updateMidiDevices (vector<string> inputs, vector<string> outputs) {
#ifdef BUILD_GUI_IMGUI
	midi_inputs.clear();
	for(auto& i : inputs) {
		midi_inputs.push_back(i);
	}
	
	midi_outputs.clear();
	for(auto& o : outputs) {
		midi_outputs.push_back(o);
	}
#endif
}

void GUI::updatePresets(vector<PresetGUIItem> presets) {
#ifdef BUILD_GUI_IMGUI
	this->presets.clear();
	for(auto& p : presets) {
		this->presets.push_back(p);
	}
#endif
}
