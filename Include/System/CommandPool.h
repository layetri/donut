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

#include <Global.h>
#include <ext/ctre.h>
#include <System/ParameterStore.h>
#include <System/PresetEngine.h>
#include <System/GUI.h>

using namespace std;

enum CommandNamespace {
	cns_Main,
	cns_Sequencer,
};

struct Command {
	Command(queue<Event*>*, GUI*, ParameterPool*, PresetEngine*, bool* running);

    virtual bool handleIfMatch(string command) = 0;
    virtual void handle(string command) = 0;
	virtual HelpItem getHelpText() = 0;

	protected:
		queue<Event *>* event_queue;
		ParameterPool* parameters;
		PresetEngine* presetEngine;
		bool* running;
		GUI* gui;
		static constexpr auto pattern = "";
		HelpItem helpText = {"command", "text"};
};

class CommandPool {
	public:
		CommandPool(queue<Event*>* event_queue, GUI* gui, ParameterPool* parameters, PresetEngine* presetEngine, bool* running);
		~CommandPool();

		void registerCommand(Command *c);
		void handleCommand(string command);
		static string color(const string&, int);

	private:
		vector<Command *> commands;
		GUI* gui;
};


#endif //DONUT_COMMANDPOOL_H
#endif
