//
// Created by Daniël Kamp on 08/06/2021.
//

#ifndef KARPLUSSTRONG_EXCITER_H
#define KARPLUSSTRONG_EXCITER_H

#include "Global.h"

#if !defined(PLATFORM_DARWIN_X86)
  #include <Arduino.h>
#elif defined(PLATFORM_DARWIN_X86)
  #include <cmath>
  #include <random>
  #include <cstdint>
  #include <ctime>
#endif

#include <string>
#include <vector>

#include "Buffer.h"

// Provide a base struct for various exciters
struct Exciter {
  virtual ~Exciter() {};
  virtual bool isOfType(std::string type) { return false; };
  virtual void excite(int length, int samplerate, Buffer* buffer) {};
};

// Declare a sine wave based exciter
struct SineExciter : public Exciter {
  bool isOfType(std::string type) override {
    return type == "sine";
  }
  void excite(int length, int samplerate, Buffer *buffer) override {
    float phase_step = 440.0 / samplerate;
    float phase = 0.0;

    for(int i = 0; i < length; i++) {
      int16_t smp = M_PI * 2 * phase * 32768;
      smp = (buffer->readAhead(i - 1) + smp) * 0.5;
      buffer->writeAhead(smp, i);

      phase += phase_step;
      if(phase > 1.0) phase -= 1.0;
    }
  }
};

// Declare a square wave based exciter
struct SquareExciter : public Exciter {
  bool isOfType(std::string type) override {
    return type == "square";
  }

  void excite(int length, int samplerate, Buffer *buffer) override {

  }
};

// Declare a noise burst based exciter
struct NoiseExciter : public Exciter {
  bool isOfType(std::string type) override {
    return type == "noise";
  }

  void excite(int length, int samplerate, Buffer *buffer) override {
    for (int i = 0; i < length; i++) {
      #ifdef PLATFORM_DARWIN_X86
        static std::default_random_engine e;
        std::uniform_real_distribution<> dist(-32768, 32768);
        auto smp = (int16_t) dist(e);
      #elif defined(PLATFORM_TEENSY40)
        auto smp = (int16_t) random(-32768, 32768);
      #endif

      buffer->writeAhead(smp, i);
    }
  }
};

// Declare an impulse based exciter
struct ImpulseExciter : public Exciter {
  bool isOfType(std::string type) override {
    return type == "impulse";
  }

  void excite(int length, int samplerate, Buffer *buffer) override {
    for(int i = 0; i < 10; i++) {
      int16_t smp;
      if(i % 2) {
        smp = 32767;
      } else {
        smp = -32767;
      }
      buffer->writeAhead(smp, i);
    }
  }
};

// Provide an interface for all the different exciters
struct ExcitationInterface {
  ExcitationInterface(int n_samplerate, Buffer* n_buffer) {
    samplerate = n_samplerate;
    buffer = n_buffer;

    import(new SineExciter);
    import(new ImpulseExciter);
    import(new NoiseExciter);

    // TODO: implement various exciters
    import(new SquareExciter);
  }
  ~ExcitationInterface() {
    for(auto *e : exciters) {
      delete e;
    }
  }

  void excite(const std::string& type, int length) {
    for (auto *e : exciters) {
      if (e->isOfType(type)) {
        e->excite(length, samplerate, buffer);
      }
    }
  };
  void import(Exciter* exciter)  {
    exciters.push_back(exciter);
  }
private:
  Buffer* buffer;
  std::vector<Exciter*> exciters;

  int samplerate;
};

#endif //KARPLUSSTRONG_EXCITER_H
