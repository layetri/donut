//
// Original by Ira Helfferich [../effects/ira/src/Pan.h]
// Extended and finished by Daniël Kamp
//

#ifndef _PANNING_INTERFACE_H_
#define _PANNING_INTERFACE_H_

// Include global dependencies
#include "Global.h"

// Include project dependencies
#include "Buffer.h"
#include "Analyzer.h"
#include "SpatialObject.h"

// Include platform dependencies
#if defined(PLATFORM_TEENSY_40)
  #include <Arduino.h>
#elif defined(PLATFORM_DARWIN_X86)
  	#include <cmath>
  	#include <cstdint>
	#include <vector>
#endif

using namespace std;

// Interface to all general panning functions
class PanningInterface {
  public:
	  PanningInterface(vector<Buffer*>* input_buffers, Buffer* output_left, Buffer* output_right);
	  ~PanningInterface();

	  void process();
	  void tick();
  protected:
    SpatialObject* objects[NUMBER_OF_VOICES];
    Buffer* output_left;
    Buffer* output_right;
};

#endif