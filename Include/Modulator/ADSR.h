#pragma once

#include <Modulator/Modulator.h>
#include <DSP/FloatBuffer.h>

using namespace std;

enum EnvPhase {
	ph_idle,
	ph_attack,
	ph_decay,
	ph_sustain,
	ph_release
};

class ADSR2 : public Modulator {
	public:
		ADSR2(Parameter*, Parameter*, Parameter*, Parameter*, ModID, string, uint8_t);
		~ADSR2();

		void regen();
		void process() override;
		void start(float velocity) override;
		void noteOff() override;
	private:
		Parameter *attack, *decay, *sustain, *release;
		float velocity;

		double a_step, d_step, r_step;
		EnvPhase phase;

		int position;
		float multiplier;
};