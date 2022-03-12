//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/CommandPool.h"
#ifdef USE_COMMAND_POOL

string CommandPool::color(const string& text, int color) {
	return "\033[" + std::to_string(color) + "m" + text + "\033[0m";
}

Command::Command(queue<Event*>* e_q, ParameterPool* parameters, PresetEngine* presetEngine, bool* r) {
	this->event_queue = e_q;
	this->running = r;
	this->parameters = parameters;
	this->presetEngine = presetEngine;
}


// CommandPool definitions
CommandPool::~CommandPool() {}

void CommandPool::registerCommand(Command *c) {
	this->commands.push_back(c);
}
void CommandPool::handleCommand(string command) {
	for(auto& cmd : commands) {
		if(cmd->handleIfMatch(command)) {
			return;
		}
	}
	cout << "[" << color("x", 31) << "] Command does not exist." << endl;
}

// ============================ //
//		Command Handlers		//
// ============================ //
struct handleFilterCutoffGlobal : public Command {
	explicit handleFilterCutoffGlobal(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			auto frequency = std::stoi(m.get<1>().to_string());
			cout <<"Setting cutoff to " << CommandPool::color(to_string(frequency), 36) << endl;
			event_queue->push(new Event{e_Control, 21 + p_Filter_Cutoff, (uint16_t) frequency});
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^cutoff\s([0-9]+)$)"};
};

struct handleFilterResonanceGlobal : public Command {
	explicit handleFilterResonanceGlobal(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		if (auto m = ctre::match<pattern>(command)) {
			auto amount = std::stoi(m.get<1>().to_string());
			event_queue->push(new Event {e_Control, 21 + p_Filter_Resonance, (uint16_t) amount});
		}
	}

	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^resonance\s([0-9]+)$)"};
};

// =======================================================================

struct handleSequence : public Command {
	explicit handleSequence(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		if(auto m = ctre::match<pattern>(command)) {
			string bunch = m.get<1>().to_string();
			static constexpr auto p2 = ctll::fixed_string {R"(^\s([^\s]+)+$)"};
			auto sequence = ctre::match<p2>(bunch);
			string steps[8];
			for(size_t i = 0; i < sequence.size(); i++) {
//				steps[i] = sequence.get<i>().to_string();
			}

//			cout << "Sequencing " << CommandPool::color(s1, 35) << ", " << CommandPool::color(s2, 35) << endl;
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^sequence(\s([0-9]+))+$)"};
};

// =======================================================================

struct handleRemap : public Command {
	explicit handleRemap(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		if(auto m = ctre::match<pattern>(command)) {
			string bunch = m.get<1>().to_string();
			if(bunch == "help") {
				for(auto& p : *parameters->getAll()) {
					printw("%s, ", p->key.c_str());
				}
				printw("...\n");
				refresh();
			} else {
				cout << "Remapping CC: " << bunch << endl;
				// Enable global remap mode
				// Next incoming MIDI CC is saved as new controller
				auto pid = parameters->translate(bunch);
				event_queue->push(new Event {e_Control, 255, static_cast<unsigned short>(pid)});
			}
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^remap\s([a-z_]+)$)"};
};

// =======================================================================

struct handleLFOAssign : public Command {
	explicit handleLFOAssign(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		if(auto m = ctre::match<pattern>(command)) {
			string lfo_num = m.get<1>().to_string();
			string param = m.get<2>().to_string();
			auto amount = m.get<3>().to_number();
			auto pid = parameters->translate(param);
//			lfo 1 assign test amount 127
			cout << "assign lfo " << CommandPool::color(lfo_num, 36) << " to param " <<
				CommandPool::color(param, 36) << " (" << amount << ")" << endl;
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^lfo\s([0-9])\sassign\s([a-z_]+)\samount\s(12[0-7]|1[01][0-9]|[0-9]?[0-9]?)$)"};
};

// =======================================================================

struct handleLFORemove : public Command {
	explicit handleLFORemove(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		if(auto m = ctre::match<pattern>(command)) {
			string bunch = m.get<1>().to_string();
			if(bunch == "help") {
				for(auto& p : *parameters->getAll()) {
					cout << p->key << ", ";
				}
				cout << "...\n";
			} else {
				cout << "Remapping CC: " << bunch << endl;
				// Enable global remap mode
				// Next incoming MIDI CC is saved as new controller
				auto pid = parameters->translate(bunch);
				event_queue->push(new Event {e_Control, 255, static_cast<unsigned short>(pid)});
			}
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^lfo\s([0-9])\sremove\s([a-z_]+)$)"};
};

// =======================================================================

struct handleStorePreset : public Command {
	explicit handleStorePreset(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		if(auto m = ctre::match<pattern>(command)) {
			string name = m.get<1>().to_string();
			presetEngine->store(name);
		}
	}
protected:
	static constexpr auto pattern = ctll::fixed_string {R"(^preset\sstore\s([a-z0-9_]+)$)"};
};

// =======================================================================

struct handleLogPresets : public Command {
	explicit handleLogPresets(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		presetEngine->log();
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^preset\slist$)"};
};

// =======================================================================

struct handleLoadPreset : public Command {
	explicit handleLoadPreset(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		if(auto m = ctre::match<pattern>(command)) {
			uint n = m.get<1>().to_number();
			presetEngine->load(n);
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^preset\sload\s([0-9]+)$)"};
};

// =======================================================================

struct handleListMIDI : public Command {
	explicit handleListMIDI(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		if(auto m = ctre::match<pattern>(command)) {
			event_queue->push(new Event {e_System, 21 + p_MIDI_List, 0});
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^midi\slist$)"};
};

// =======================================================================

struct handleSelectMIDI : public Command {
	explicit handleSelectMIDI(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		if(auto m = ctre::match<pattern>(command)) {
			uint n = m.get<1>().to_number();
			event_queue->push(new Event {e_System, 21 + p_MIDI_In, n});
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^midi\sselect\s([0-9]+)$)"};
};

// =======================================================================

struct handleProgramExit : public Command {
	explicit handleProgramExit(queue<Event *>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) : Command(event_queue, parameters, presetEngine, running) {};

	bool handleIfMatch(string command) override {
		if(ctre::match<pattern>(command)) {
			cout << "Exiting. Goodbye!" << endl;
			handle(command);
			return true;
		}
		return false;
	}

	void handle(string command) override {
		*running = false;
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(exit)"};
};

CommandPool::CommandPool(queue<Event*>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running) {
	registerCommand(new handleFilterCutoffGlobal(event_queue, parameters, presetEngine, running));
	registerCommand(new handleFilterResonanceGlobal(event_queue, parameters, presetEngine, running));
	registerCommand(new handleStorePreset(event_queue, parameters, presetEngine, running));
	registerCommand(new handleSequence(event_queue, parameters, presetEngine, running));
	registerCommand(new handleRemap(event_queue, parameters, presetEngine, running));
	registerCommand(new handleLFOAssign(event_queue, parameters, presetEngine, running));
	registerCommand(new handleLFORemove(event_queue, parameters, presetEngine, running));
	registerCommand(new handleLogPresets(event_queue, parameters, presetEngine, running));
	registerCommand(new handleLoadPreset(event_queue, parameters, presetEngine, running));
	registerCommand(new handleListMIDI(event_queue, parameters, presetEngine, running));
	registerCommand(new handleSelectMIDI(event_queue, parameters, presetEngine, running));
	registerCommand(new handleProgramExit(event_queue, parameters, presetEngine, running));
}
#endif