//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/CommandPool.h"

#ifdef USE_COMMAND_POOL

string CommandPool::color (const string &text, int color) {
	return "\033[" + std::to_string(color) + "m" + text + "\033[0m";
}

Command::Command (queue<Event *> *e_q, ParameterPool *parameters, PresetEngine *presetEngine, bool *r) {
	this->event_queue = e_q;
	this->running = r;
	this->parameters = parameters;
	this->presetEngine = presetEngine;
}


// CommandPool definitions
CommandPool::~CommandPool () {}

void CommandPool::registerCommand (Command *c) {
	this->commands.push_back(c);
}

void CommandPool::handleCommand (string command) {
	if (command == "help") {
		printw("Here's what I can do:\n");
		refresh();
		
		for (auto &cmd: commands) {
			auto line = cmd->getHelpText();
			
			int winy, winx;
			getyx(stdscr, winy, winx);
			move(winy, 4);
			
			attron(COLOR_PAIR(5));
			printw(line.command.c_str());
			attroff(COLOR_PAIR(5));
			mvprintw(winy, 40, "%s\n", line.description.c_str());
		}
		refresh();
		return;
	} else {
		for (auto &cmd: commands) {
			if (cmd->handleIfMatch(command)) {
				return;
			}
		}
	}
	cout << "[" << color("x", 31) << "] Command does not exist." << endl;
}

// ============================ //
//		Command Handlers		//
// ============================ //
struct handleSet : public Command {
	explicit handleSet (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			string p_name = m.get<1>().to_string();
			uint16_t value = m.get<2>().to_number();
			ParameterID param = parameters->translate(p_name);
			
			event_queue->push(new Event{e_Control, (uint16_t)(21 + param), value});
			
			printw("Setting ");
			attron(COLOR_PAIR(5));
			printw(p_name.c_str());
			attroff(COLOR_PAIR(5));
			printw(" to ");
			attron(COLOR_PAIR(5));
			printw(to_string(value).c_str());
			attroff(COLOR_PAIR(5));
			refresh();
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^set\s([a-z_]+)\s([0-9]+)$)"};
	HelpItem helpText = {"set <param> <value>", "Set the specified parameter to the specified amount."};
};

// =======================================================================

struct handleSequence : public Command {
	explicit handleSequence (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			string bunch = m.get<1>().to_string();
			static constexpr auto p2 = ctll::fixed_string{R"(^\s([^\s]+)+$)"};
			auto sequence = ctre::match<p2>(bunch);
			string steps[8];
			for (size_t i = 0; i < sequence.size(); i++) {
//				steps[i] = sequence.get<i>().to_string();
			}

//			cout << "Sequencing " << CommandPool::color(s1, 35) << ", " << CommandPool::color(s2, 35) << endl;
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^sequence(\s([0-9]+))+$)"};
	HelpItem helpText = {"sequence", "Build a sequence (NOT IMPLEMENTED)."};
};

// =======================================================================

struct handleRemap : public Command {
	explicit handleRemap (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			string bunch = m.get<1>().to_string();
			cout << "Remapping CC: " << bunch << endl;
			// Enable global remap mode
			// Next incoming MIDI CC is saved as new controller
			auto pid = parameters->translate(bunch);
			event_queue->push(new Event{e_Control, 255, static_cast<unsigned short>(pid)});
			
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^remap\s([a-z_]+)$)"};
	HelpItem helpText = {"remap <parameter>",
						 "Remap a MIDI control to a parameter. [param list] for a list of parameters."};
};

// =======================================================================

struct handleListParams : public Command {
	explicit handleListParams (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			for (auto &p: *parameters->getDictionary()) {
				printw("%s\n", p->key.c_str());
			}
			refresh();
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^param\slist$)"};
	HelpItem helpText = {"param list", "Lists all available parameters."};
};

// =======================================================================

struct handleLFOAssign : public Command {
	explicit handleLFOAssign (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			string lfo_num = m.get<1>().to_string();
			string param = m.get<2>().to_string();
			auto amount = m.get<3>().to_number();
//			auto pid = parameters->translate(param);
//			lfo 1 assign test amount 127
			cout << "assign lfo " << CommandPool::color(lfo_num, 36) << " to param " <<
				 CommandPool::color(param, 36) << " (" << amount << ")" << endl;
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{
		  R"(^lfo\s([0-9])\sassign\s([a-z_]+)\samount\s(12[0-7]|1[01][0-9]|[0-9]?[0-9]?)$)"};
	HelpItem helpText = {"lfo <n> assign <param> amount <amt>",
						 "Assign an LFO to a parameter and control the amount (NOT IMPLEMENTED)."};
};

// =======================================================================

struct handleLFORemove : public Command {
	explicit handleLFORemove (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			string bunch = m.get<1>().to_string();
			if (bunch == "help") {
				for (auto& p : *parameters->getDictionary()) {
					cout << p->key << ", ";
				}
				cout << "...\n";
			} else {
				cout << "Remapping CC: " << bunch << endl;
				// Enable global remap mode
				// Next incoming MIDI CC is saved as new controller
				auto pid = parameters->translate(bunch);
				event_queue->push(new Event{e_Control, 255, static_cast<unsigned short>(pid)});
			}
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^lfo\s([0-9])\sremove\s([a-z_]+)$)"};
	HelpItem helpText = {"lfo <n> remove <parameter>", "Remove an LFO from the specified parameter (NOT IMPLEMENTED)."};
};

// =======================================================================

struct handleToggleSource : public Command {
	explicit handleToggleSource (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			string source = m.get<1>().to_string();
			string state = m.get<2>().to_string();
			if (source.compare("waveshaper")) {
				event_queue->push(new Event{e_Control, 21 + p_WS_Toggle, (uint16_t) state.compare("on")});
				return;
			} else if (source.compare("wavetable")) {
				event_queue->push(new Event{e_Control, 21 + p_WT_Toggle, (uint16_t) state.compare("on")});
				return;
			} else if (source.compare("sub")) {
				event_queue->push(new Event{e_Control, 21 + p_Sub_Toggle, (uint16_t) state.compare("on")});
				return;
			}
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^source\s([a-z0-9_]+)\s(on|off)$)"};
	HelpItem helpText = {"source <name> <on/off>", "Toggle a specified sound source."};
};

// =======================================================================

struct handleTranspose : public Command {
	explicit handleTranspose (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			string source = m.get<1>().to_string();
			uint16_t semis = m.get<2>().to_number();
			
			SourceID src;
			if (source.compare("shaper_1")) {
//				src = s_WS1;
			} else if (source.compare("shaper_2")) {
//				src = s_WS2;
			} else if (source.compare("wavetable")) {
				src = s_WT1;
			} else if (source.compare("sub")) {
				src = s_Sub;
			}
			event_queue->push(new Event{e_Control, 0, semis, c_Transpose, src});
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^transpose\s([a-z0-9_]+)\s([0-9]+)$)"};
	HelpItem helpText = {"source <name> <on/off>", "Toggle a specified sound source."};
};

// =======================================================================

struct handleStorePreset : public Command {
	explicit handleStorePreset (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			string name = m.get<1>().to_string();
			presetEngine->store(name);
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^preset\sstore\s([a-z0-9_]+)$)"};
	HelpItem helpText = {"preset store <name>", "Store the current settings as a preset."};
};

// =======================================================================

struct handleLogPresets : public Command {
	explicit handleLogPresets (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		presetEngine->log();
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^preset\slist$)"};
	HelpItem helpText = {"preset list", "List all available presets."};
};

// =======================================================================

struct handleLoadPreset : public Command {
	explicit handleLoadPreset (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			uint n = m.get<1>().to_number();
			presetEngine->load(n);
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^preset\sload\s([0-9]+)$)"};
	HelpItem helpText = {"preset load <name>", "Load the specified preset."};
};

// =======================================================================

struct handleSplit : public Command {
	explicit handleSplit (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			uint16_t n = m.get<1>().to_number();
			event_queue->push(new ControlEvent {c_Split, n});
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^split\s(12[0-7]|1[01][0-9]|[0-9]?[0-9]?)$)"};
	HelpItem helpText = {"split <key>", "Split the keyboard at the specified key."};
};

// =======================================================================

struct handleListMIDI : public Command {
	explicit handleListMIDI (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			event_queue->push(new Event{e_System, 21 + p_MIDI_List, 0});
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^midi\slist$)"};
	HelpItem helpText = {"midi list", "List available MIDI input devices."};
};

// =======================================================================

struct handleSelectMIDIIn : public Command {
	explicit handleSelectMIDIIn (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			uint16_t n = m.get<1>().to_number();
			event_queue->push(new Event{e_System, 21 + p_MIDI_In, n});
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^midi\sin\s([0-9]+)$)"};
	HelpItem helpText = {"midi in <device>", "Select a MIDI input device."};
};

// =======================================================================

struct handleSelectMIDIOut : public Command {
	explicit handleSelectMIDIOut (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	void handle (string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			uint16_t n = m.get<1>().to_number();
			event_queue->push(new Event{e_System, 21 + p_MIDI_Out, n});
		}
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(^midi\sout\s([0-9]+)$)"};
	HelpItem helpText = {"midi out <device>", "Select a MIDI output device."};
};

// =======================================================================

struct handleProgramExit : public Command {
	explicit handleProgramExit (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) : Command(event_queue, parameters, presetEngine, running) {};
	
	HelpItem getHelpText () override {
		return helpText;
	}
	
	bool handleIfMatch (string command) override {
		if (ctre::match<pattern>(command)) {
			cout << "Exiting. Goodbye!" << endl;
			handle(command);
			return true;
		}
		return false;
	}
	
	void handle (string command) override {
		*running = false;
	}

protected:
	static constexpr auto pattern = ctll::fixed_string{R"(exit)"};
	HelpItem helpText = {"exit", "Quit Donut."};
};

CommandPool::CommandPool (queue<Event *> *event_queue, ParameterPool *parameters, PresetEngine *presetEngine, bool *running) {
	registerCommand(new handleSet(event_queue, parameters, presetEngine, running));
	registerCommand(new handleListParams(event_queue, parameters, presetEngine, running));
	registerCommand(new handleStorePreset(event_queue, parameters, presetEngine, running));
	registerCommand(new handleSequence(event_queue, parameters, presetEngine, running));
	registerCommand(new handleRemap(event_queue, parameters, presetEngine, running));
	registerCommand(new handleLFOAssign(event_queue, parameters, presetEngine, running));
	registerCommand(new handleLFORemove(event_queue, parameters, presetEngine, running));
	registerCommand(new handleToggleSource(event_queue, parameters, presetEngine, running));
	registerCommand(new handleLogPresets(event_queue, parameters, presetEngine, running));
	registerCommand(new handleLoadPreset(event_queue, parameters, presetEngine, running));
	registerCommand(new handleSplit(event_queue, parameters, presetEngine, running));
	registerCommand(new handleListMIDI(event_queue, parameters, presetEngine, running));
	registerCommand(new handleSelectMIDIIn(event_queue, parameters, presetEngine, running));
	registerCommand(new handleSelectMIDIOut(event_queue, parameters, presetEngine, running));
	registerCommand(new handleProgramExit(event_queue, parameters, presetEngine, running));
}

#endif