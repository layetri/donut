#include "../..dsp//Header/FloatBuffer.h"

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
            (sample_num)
    }
};

class ADSR {
    public:
        ADSR(EnvelopeParameters*);
        ~ADSR();

        void process();
        void start();
        void reset();

        void refresh();
    private:
        FloatBuffer* mod_buffer;
        bool running;
        bool has_started;

        // Store envelope values in samples
        EnvelopeParameters* params;
};