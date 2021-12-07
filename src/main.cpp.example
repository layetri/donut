#include <iostream>

#include "Header/Global.h"
#include "Header/Buffer.h"
#include "Header/DelayLine.h"

#include "Header/Amplitude.h"
#include "Header/PanningInterface.h"

// Compilation target TEENSY40 hasn't been implemented yet
// DO NOT USE!
#if defined(PLATFORM_TEENSY_40)
  #include "Header/AudioInput.h"
  #include <Arduino.h>

  #define samplerate 44100
  #define angleDeg 45

  //int setup() {
  //
  //}
  AudioInput input1(0);
  AudioInput input2(1);

  Buffer inputL(samplerate * 10, "input");
  Buffer inputR(samplerate * 10, "input");

  Buffer outputL(samplerate * 10, "output");
  Buffer outputR(samplerate * 10, "output");

  LowPassFilter filterL(5000.0, samplerate, &inputL, &outputL);
  LowPassFilter filterR(5000.0, samplerate, &inputR, &outputR);
  filterL.setFrequency(500.0);
  filterR.setFrequency(500.0);

  int main() {
    float panning = -1;
    Pan pan;

    panning = panning + 0.00001;
    if(panning > 1) {
      panning = -1;
    }

    inputL.write(input1.getSample());
    inputR.write(input2.getSample());

    outputL.write(filterL.process() * pan.gainCalL(angleDeg, panning));
    outputR.write(filterR.process() * pan.gainCalR(angleDeg, panning));
  }
#elif defined(PLATFORM_DARWIN_X86)
  #include "Header/jack_module.h"

  int main() {
    int samplerate = 44100;

    // Do Jack setup
    JackModule jack;
    try {
      jack.init("_localrun1");
      samplerate = jack.getSamplerate();
    } catch(std::bad_function_call e) {
      std::cout << e.what() << std::endl;
    }

    if (samplerate == 0) {
      samplerate = 44100;
    }

    // Create input array for 6 input channels
    Buffer* inputs[NUM_INPUTS];
    for(int i = 0; i < NUM_INPUTS; i++) {
      inputs[i] = new Buffer(samplerate, "input");
    }

    // Initialize output busses and FX
    Buffer revBusL(samplerate * 10, "revbus_L");
    Buffer revBusR(samplerate * 10, "revbus_R");
    Buffer outputL(samplerate, "output_L");
    Buffer outputR(samplerate, "output_R");
    Buffer output(samplerate, "intermediate");

    DelayLine dl_L(9000, 0.85, &revBusL);
    DelayLine dl_R(8900, 0.85, &revBusR);


    // Initialize panning component
    PanningInterface panner(inputs, &revBusL, &revBusR);
    Amplitude amplitude(inputs[0], &output);

    // Assign the Jack callback function
    jack.onProcess = [&panner, &amplitude, &inputs,
                      &revBusL, &revBusR,
                      &outputL, &outputR,
                      &dl_L, &dl_R]
        (jack_default_audio_sample_t **inBufArray,
        jack_default_audio_sample_t *outBuf_L,
        jack_default_audio_sample_t *outBuf_R,
        jack_nframes_t nframes
      ) {
        for(unsigned int i = 0; i < nframes; i++) {
          for(int j = 0; j < NUM_INPUTS; j++) {
            inputs[j]->write(inBufArray[j][i] * 32768.0);
            inputs[j]->tick();
          }

          verbose(amplitude.DetectOnsets());

          // Run the panning algorithm
          panner.process();

          // Process the delay lines
          outputL.write(dl_L.process());
          outputR.write(dl_R.process());

          // Get the processed samples and write to output
          outBuf_L[i] = (revBusL.getCurrentSample() * 0.3 +
              revBusL.readBack(1) * 0.3 +
              outputL.getCurrentSample() * 0.2 +
              outputL.readBack(1) * 0.2) / 32768.0;
          outBuf_R[i] = (revBusR.getCurrentSample() * 0.3 +
              revBusR.readBack(1) * 0.3 +
              outputR.getCurrentSample() * 0.2 +
              outputR.readBack(1) * 0.2) / 32768.0;

          panner.tick();

          dl_L.tick();
          dl_R.tick();
        }
        return 0;
    };
    jack.autoConnect();

    bool running = true;
    while(running) {
      char cmd;
      std::cout << "Command: ";
      std::cin >> cmd;

      switch(cmd) {
        case('q') : {
          running = false;
        }
      }
    }
  }
#endif
