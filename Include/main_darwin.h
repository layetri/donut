//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_MAIN_DARWIN_H
#define DONUT_MAIN_DARWIN_H

#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>

#include <ext/jack_module.h>
#include <ext/RtMidi.h>

#include <DSP/Buffer.h>
#include <DSP/StereoOutputBuffer.h>

#include <Effect/AutoMaster.h>
#include <Source/Sampler.h>
#include <Source/Particles.h>

#include <System/PresetEngine.h>
#include <System/ParameterStore.h>
#include <System/CommandPool.h>
#include <System/NoteHandler.h>
#include <System/Voice.h>
#include <System/ControlMap.h>
#include <System/Tables.h>
#include <System/ModMatrix.h>
#include <System/GUI.h>
#include <System/SampleLibrary.h>
#include <System/Scheduler.h>
#include <System/DeveloperUtility.h>

using namespace std;

void program();
void switchMidiInputs(RtMidiIn*, uint, GUI*);
void switchMidiOutputs(RtMidiOut*, uint, GUI*);
void listMidiDevices(RtMidiIn*, RtMidiOut*, GUI*);

#endif //DONUT_MAIN_DARWIN_H
