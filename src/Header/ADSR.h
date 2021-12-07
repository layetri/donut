#include "Modulator.h"
#include "FloatBuffer.h"

#include <iostream>
using namespace std;

struct EnvelopeParameters {
    int attack;
    int decay;
    float sustain;
    int release;

    int total_length() {
        return attack + decay + release;
    }

    bool can_sustain(int sample_num) {
        return sample_num > attack + decay;
    }

    int get_current_value(int sample_num) {
        return (sample_num < attack) * ((SAMPLE_MAX_VALUE / attack) * sample_num) +
            (sample_num < attack + decay) * ((SAMPLE_MAX_VALUE * (1.0 - sustain) / decay) * (sample_num - attack)) +
            (sample_num);
    }
};

enum EnvPhase {
	ph_idle,
	ph_attack,
	ph_decay,
	ph_sustain,
	ph_release
};

class ADSR2 {
	public:
		ADSR2(int, int, float, int);
		~ADSR2();

		void regen();
		void step();
		void start();
		void noteOff();
		double getMultiplier();

		void setAttack(int attack);
		void setDecay(int decay);
		void setSustain(float sustain);
		void setRelease(int release);

	private:
		int attack;
		int decay;
		float sustain;
		int release;

		double a_step, d_step, r_step;
		EnvPhase phase;

		int counter;
		double multiplier = 0.0;
};