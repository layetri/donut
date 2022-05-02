## EL-ONE (Donut)
What features does your synth have? Yes.

Donut started as a passion-project-turned-dream-synth. The current outline of features includes:
- 5 Sources:
  - Wavetable oscillator
  - Additive waveshaper
  - Slimmed down [tensions](https://github.com/layetri/karplusstrong) implementation
  - Particles: a granular experience
  - Sampler instrument
- 4 Modulators:
  - Morphing LFO
  - 4-point envelope (ADSR, point-to-point)
  - Random generator with various quantization features
  - Sequencer
- 4 Filters:
  - Lowpass filter
  - Highpass filter
  - Bandpass filter
  - Comb filter
- 4 Effects:
  - Reverb
  - Stereo delay
  - Wavefolder
  - Glitch

### Demos
I made a few demos using the software prototype. They are listed here:
- [devlog I](https://www.youtube.com/watch?v=OLlrPg6xvhI)
- [devlog II](https://www.youtube.com/watch?v=XgsYKxt4CRc)
- [devlog III](https://www.youtube.com/watch?v=7CKzSvlqjKY)

### Software
The Donut firmware for EL-ONE is completely custom, written in C++, and can be compiled to run on macOS and Linux. Future releases will include hardware implementation, right now it only runs as a CLI application. Donut uses `jackd` as its audio backend.

A web-based patch editor for the system is also available. It can be found at [editor.donut.layetri.net](https://editor.donut.layetri.net).

### Hardware
EL-ONE is based around the Raspberry Pi 4B. Hardware design is currently in progress, however a few details are listed here:
- 7" central touchscreen
- 4 separate sections for the different categories (sources, modulators, filters, effects)
  - One rotary encoder for circular input
  - One 0.9" OLED display
  - Four or five tactile switches (with LEDs?)
- 61 key keybed (salvaged from a Yamaha PSR-280)
- 16-bit DAC (TDA1543A, unless I can find something better that isn't out of stock)

### Getting the whole thing
The software implementation can be found in this repository. Once I finish the hardware design, I will provide it for free through my website, however I will ask anyone who wants to build it to leave me a little message (in case anyone ever sees this or wants one for themselves, I'd love to know). 
#### Build instructions
To build the firmware available through this repository, you should first make sure you have all the necessary dependencies installed. These are the requirements for building Donut:
- `ncurses`, `libncurses` or your platform's equivalent
- `libjack2-dev` headers available globally
- `gcc` version x and up
- `cmake` for completeness' sake

First, generate build files from the project root:
```shell
  cmake .
```
Then, use CMake to build the project:
```shell
  cmake . --build
```

Finally, to run Donut, first start `jackd`. Once JACK is running, run the program from the project root folder:
```shell
  ./CMakeFiles/donut
```
NOTE 1: make sure to run Donut from the project root folder! This is where it looks for its runtime resource files (like wavetables and presets).

NOTE 2 (Linux only): before running Donut, make sure there's a MIDI connection available through something like `a2jmidid`.

### Thanks and stuff
- The default piano sample was generously provided by Björn Colin