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

### Software
The Donut firmware for EL-ONE is completely custom, written in C++, and can be compiled to run on macOS and Linux. Future releases will include hardware implementation, right now it only runs as a CLI application. Donut uses `jackd` as its audio backend.

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