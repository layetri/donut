//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_MAIN_DARWIN_H
#define DONUT_MAIN_DARWIN_H

#include <ext/jack_module.h>
#include <ext/RtMidi.h>
#include <curses.h>

#include <System/PresetEngine.h>
#include <System/ParameterStore.h>
#include <System/CommandPool.h>
#include <DSP/Buffer.h>
#include <System/NoteHandler.h>
#include <System/Voice.h>
#include <System/ControlMap.h>
#include <System/Tables.h>
#include <Effect/AutoMaster.h>
#include <System/ModMatrix.h>
#include <System/GUI.h>

#include <vector>
#include <string>
#include <sstream>

#include <unistd.h>
#include <cstdlib>
#include <queue>
#include <thread>
#include <mutex>

using namespace std;

void program();
void switchMidiInputs(RtMidiIn*, uint);
void switchMidiOutputs(RtMidiOut*, uint);
void listMidiDevices(RtMidiIn*, RtMidiOut*);

#endif //DONUT_MAIN_DARWIN_H
