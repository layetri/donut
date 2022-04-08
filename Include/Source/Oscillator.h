#pragma once
#include <Global.h>
#include <DSP/Buffer.h>
#include <Source/Source.h>

#ifndef PLATFORM_EMBEDDED
    #include <math.h>
    #define TWO_PI (2 * M_PI)
#endif

class Oscillator : public Source {
    public:
        Oscillator(float frequency, ParameterPool* params, uint8_t voice_id);
        ~Oscillator();

		void setBaseFrequency(float base_frequency);
        void setFrequency(float frequency) override;
		float getFrequency();
		void incrementPhase();
        void calculatePhaseStep();

        void pitch(uint8_t midi_num) override;

    protected:
		int note = 0;
};