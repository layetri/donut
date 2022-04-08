//
// Created by Daniël Kamp on 09/11/2021.
//
#define USE_COMMAND_POOL

#ifdef USE_COMMAND_POOL
#ifndef DONUT_COMMANDPOOL_H
#define DONUT_COMMANDPOOL_H

#include <string>
#include <functional>
#include <vector>
#include <queue>
#include <curses.h>

#include <Global.h>
#include <ext/ctre.h>
#include "ParameterStore.h"
#include "PresetEngine.h"

using namespace std;

enum CommandNamespace {
	cns_Main,
	cns_Sequencer,
};

struct Command {
	Command(queue<Event*>*, ParameterPool*, PresetEngine*, bool* running);

    virtual bool handleIfMatch(string command) = 0;
    virtual void handle(string command) = 0;
	virtual HelpItem getHelpText() = 0;

	protected:
		queue<Event *>* event_queue;
		ParameterPool* parameters;
		PresetEngine* presetEngine;
		bool* running;
		static constexpr auto pattern = "";
		HelpItem helpText = {"command", "text"};
};

class CommandPool {
	public:
		CommandPool(queue<Event*>* event_queue, ParameterPool* parameters, PresetEngine* presetEngine, bool* running);
		~CommandPool();

		void registerCommand(Command *c);
		void handleCommand(string command);
		static string color(const string&, int);

	private:
		vector<Command *> commands;
};


#endif //DONUT_COMMANDPOOL_H
#endif
