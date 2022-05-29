//
// Created by Daniël Kamp on 12/04/2022.
//

#include "System/GUI.h"

mutex mtx2;

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GUI::GUI (ParameterPool* parameters, ModMatrix* mod, queue<Event*>* event_queue) {
	this->parameters = parameters;
	this->event_queue = event_queue;
	this->mod = mod;
	
	for(auto& p : *parameters->getDictionary()) {
		if(p->key.find("amount") != string::npos) {
			mix_controls.push_back(parameters->get(p->pid, 0));
		} else {
			voice_controls.push_back(parameters->get(p->pid, 0));
		}
	}
//	this->commands = commands;
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
		// Setup window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
			exit(1);
		
		// Decide GL+GLSL versions
		#if defined(IMGUI_IMPL_OPENGL_ES2)
			// GL ES 2.0 + GLSL 100
			const char* glsl_version = "#version 100";
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		#elif defined(__APPLE__)
			// GL 3.2 + GLSL 150
			const char* glsl_version = "#version 150";
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);            // Required on Mac
		#endif
	
		// Create window with graphics context
		window = glfwCreateWindow(1280, 720, "DonutUI", NULL, NULL);
		if (window == NULL)
			exit(1);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable vsync
		
		// Init ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		
		io = &ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
	
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
	#endif
}

GUI::~GUI () {
	// Destroy the GUI.
	#if defined(BUILD_GUI_NCURSES)
		cout << "\x1B[2J\x1B[H";
		curs_set(1);
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
		log.push_back(line);
		if(log.size() > log_size) {
			log.pop_front();
		}
	#endif
}

void GUI::process() {
	// This is where general GUI elements go (like voice count, midi io, etc)
}

void GUI::loop() {
	#ifdef BUILD_GUI_IMGUI
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		// Logging console
		{
			ImGui::Begin("Application Console");
			for(basic_string<char> line : log) {
				ImGui::Text("%s", line.c_str());
			}
			ImGui::End();
		}
		
		// Output graph
		{
			ImGui::Begin("Oscilloscope");
			ImGui::PlotLines("Left", plotL, plot_size, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
			ImGui::PlotLines("Right", plotR, plot_size, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
			ImGui::End();
		}
		
		// Voice controls
		{
			ImGui::Begin("Voice controls");
			for(auto& v : voice_controls) {
				auto name = parameters->translate(v->pid);
				ImGui::SliderFloat(name.substr(0, name.find("_amount")).c_str(), &v->value, v->min, v->max);
				if(ImGui::IsItemEdited()) {
					event_queue->push(new Event{e_Control, (uint16_t)(21 + v->pid), (uint16_t) (127.0f * (v->value / (v->max - v->min)))});
				}
			}
			ImGui::End();
		}
		
		// Modulation Matrix
		{
			ImGui::Begin("Mod Matrix");
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
					ImGui::Text("%s", p->key.c_str());
					
					col = 1;
					for (auto& m : *mod->getDict()) {
						float v = 0.0f;
						ImGui::TableSetColumnIndex(col);
						ImGui::SliderFloat("", &v, 0.0f, 1.0f);
						if(ImGui::IsItemEdited()) {
							mod->setOrCreate(m.mid, p->pid, v);
						}
						col++;
					}
				}
				ImGui::EndTable();
			}
			ImGui::End();
		}
		
		// Mixer
		{
			ImGui::Begin("Mixer");
			for(int i = 0; i < mix_controls.size(); i++) {
				if(i > 0) ImGui::SameLine();
				
				ImGui::VSliderFloat(parameters->translate(mix_controls[i]->pid).c_str(), ImVec2(18, 160), &mix_controls[i]->value, 0.0f, 1.0f);
				if(ImGui::IsItemEdited()) {
					event_queue->push(new Event{e_Control, (uint16_t)(21 + mix_controls[i]->pid), (uint16_t) (127.0f * mix_controls[i]->value)});
				}
			}
			ImGui::End();
		}
		
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(window);
	}
	#endif
}

int GUI::cleanup() {
	#ifdef BUILD_GUI_IMGUI
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		
		glfwDestroyWindow(window);
		glfwTerminate();
	#endif
	
	return 1;
}

//void GUI::refreshDisplayValues() {
//
//}

void GUI::stereoPlot(float* left, float* right, uint size) {
	this->plotL = left;
	this->plotR = right;
	this->plot_size = size;
}