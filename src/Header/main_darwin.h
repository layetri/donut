//
// Created by Daniël Kamp on 09/11/2021.
//

#ifndef DONUT_MAIN_DARWIN_H
#define DONUT_MAIN_DARWIN_H

#include "jack_module.h"
#include "RtMidi.h"

#include "Buffer.h"
#include "NoteHandler.h"
#include "Voice.h"
//#include "PanningInterface.h"

#include <vector>
#include <string>

#include <unistd.h>
#include <cstdlib>
#include <queue>
#include <thread>
#include <mutex>

using namespace std;

void program();

#endif //DONUT_MAIN_DARWIN_H
