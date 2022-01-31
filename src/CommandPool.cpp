//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/CommandPool.h"
#ifdef USE_COMMAND_POOL

string CommandPool::color(const string& text, int color) {
	return "\033[" + std::to_string(color) + "m" + text + "\033[0m";
}

Command::Command(queue<Event*>* e_q, ParameterPool* parameters, bool* r) {
	this->event_queue = e_q;
	this->running = r;
	this->parameters = parameters;
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
	explicit handleFilterCutoffGlobal(queue<Event *>* event_queue, ParameterPool* parameters, bool* running) : Command(event_queue, parameters, running) {};

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
			event_queue->push(new Event{21 + p_Filter_Cutoff, (uint16_t) frequency});
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^cutoff\s([0-9]+)$)"};
};

struct handleFilterResonanceGlobal : public Command {
	explicit handleFilterResonanceGlobal(queue<Event *>* event_queue, ParameterPool* parameters, bool* running) : Command(event_queue, parameters, running) {};

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
			event_queue->push(new Event {21 + p_Filter_Resonance, (uint16_t) amount});
		}
	}

	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^resonance\s([0-9]+)$)"};
};

// =======================================================================

struct handlePresetStore : public Command {
	explicit handlePresetStore(queue<Event *>* event_queue, ParameterPool* parameters, bool* running) : Command(event_queue, parameters, running) {};

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
			cout << "Storing preset " << CommandPool::color(name, 35) << "." << endl;
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^preset\sstore\s(.{1,})$)"};
};

// =======================================================================

struct handleSequence : public Command {
	explicit handleSequence(queue<Event *>* event_queue, ParameterPool* parameters, bool* running) : Command(event_queue, parameters, running) {};

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
	explicit handleRemap(queue<Event *>* event_queue, ParameterPool* parameters, bool* running) : Command(event_queue, parameters, running) {};

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
				event_queue->push(new Event {255, static_cast<unsigned short>(pid)});
			}
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^remap\s([a-z_]+)$)"};
};

// =======================================================================

struct handleLFOAssign : public Command {
	explicit handleLFOAssign(queue<Event *>* event_queue, ParameterPool* parameters, bool* running) : Command(event_queue, parameters, running) {};

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
	explicit handleLFORemove(queue<Event *>* event_queue, ParameterPool* parameters, bool* running) : Command(event_queue, parameters, running) {};

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
				event_queue->push(new Event {255, static_cast<unsigned short>(pid)});
			}
		}
	}
	protected:
		static constexpr auto pattern = ctll::fixed_string {R"(^lfo\s([0-9])\sremove\s([a-z_]+)$)"};
};

// =======================================================================

struct handleProgramExit : public Command {
	explicit handleProgramExit(queue<Event *>* event_queue, ParameterPool* parameters, bool* running) : Command(event_queue, parameters, running) {};

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

CommandPool::CommandPool(queue<Event*>* event_queue, ParameterPool* parameters, bool* running) {
	registerCommand(new handleFilterCutoffGlobal(event_queue, parameters, running));
	registerCommand(new handleFilterResonanceGlobal(event_queue, parameters, running));
	registerCommand(new handlePresetStore(event_queue, parameters, running));
	registerCommand(new handleSequence(event_queue, parameters, running));
	registerCommand(new handleRemap(event_queue, parameters, running));
	registerCommand(new handleLFOAssign(event_queue, parameters, running));
	registerCommand(new handleLFORemove(event_queue, parameters, running));
	registerCommand(new handleProgramExit(event_queue, parameters, running));
}
#endif