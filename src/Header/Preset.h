//
// Created by Daniël Kamp on 02/06/2021.
//

#ifndef KARPLUSSTRONG_PRESET_H
#define KARPLUSSTRONG_PRESET_H

#include "Global.h"
#include <string>

#define NUM_PRESETS 8
#if !defined(PLATFORM_DARWIN_X86)

#elif defined(PLATFORM_DARWIN_X86)
  #include <cmath>
  #include <string>
  #include <sstream>
#include <utility>
#endif

struct Preset {
    float feedback;
    float dampening;
    int exciter;
    std::string name;

    // Provide a one-stop setup function
    void setup(float fb, float damp, int exc, std::string nm) {
      name = std::move(nm);
      feedback = fb;
      dampening = damp;
      exciter = exc;
    }
};

class PresetEngine {
public:
    PresetEngine() {
      iterator = 0;
      dial = 0;
      factor = 0;
      preset = 0;
      step = (100.0 / NUM_PRESETS);
    };

    ~PresetEngine() {
      for (auto &pr : presets) {
        delete pr;
      }
    };

    void import(float n_feedback, float n_dampening, int n_exciter, std::string n_name="User") {
      if (iterator < NUM_PRESETS) {
        presets[iterator] = new Preset;
        presets[iterator]->setup(n_feedback, n_dampening, n_exciter, n_name);

        #if defined(DEVMODE)
          verbose("preset added at:");
          verbose(iterator);
        #endif
      }
      iterator++;

    };

    // For working with dynamically imported presets:
    //  check if the next preset is set.
    bool rangeTest(float value) const {
      if (value > step * preset) {
        return preset + 1 < iterator;
      }
      return true;
    }

    //
    void rotate(int amount) {
      set((float) (dial + amount));
    }

    // Interpolate between different presets when a value is set.
    void set(float value) {
      if (value != dial) {
        if (0 < value && value < 100 && rangeTest(value)) {
          // Find step per preset
          dial = value;

          // Calculate what the next preset will be
          preset = ceil(dial / step);
          // Calculate an interpolation factor
          factor = (dial - ((preset - 1.0) * step)) / step;

          // Interpolate from one preset to the next
          feedback = factor * presets[preset]->feedback + (1 - factor) * presets[preset - 1]->feedback;
          dampening = factor * presets[preset]->dampening + (1 - factor) * presets[preset - 1]->dampening;

          // Switch the exciter preset (no interpolation here, sadly)
          if (factor > 0.5) {
            exciter = presets[preset]->exciter;
          } else {
            exciter = presets[preset - 1]->exciter;
          }
        } else {
          // Throw a warning if the requested move is impossible
          #ifdef DEVMODE
            verbose("Preset index out of range, keeping latest setting.");
          #endif
        }

        #if defined(DEVMODE) && defined(PLATFORM_DARWIN_X86)
          verbose(getPresetInfoString());
        #endif
      }
    }

    Preset getPreset() {
      Preset rtn = {feedback, dampening, exciter, presets[preset]->name};
      return rtn;
    }

    #if defined(DEVMODE) && defined(PLATFORM_DARWIN_X86)
      std::string getPresetInfoString() const {
        std::ostringstream oss;
        oss << (1 - factor) * 100 << "% preset " << preset - 1 << ", " << factor * 100 << "% preset " << preset;
        return oss.str();
      }
    #endif
  private:
    Preset* presets[NUM_PRESETS];
    int iterator;
    float dial;

    int preset;
    float factor;
    float step;

    float feedback;
    float dampening;
    int exciter;
};

#endif //KARPLUSSTRONG_PRESET_H
