#include "Header/Amplitude.h"

Amplitude::Amplitude(Buffer* input_buffer, Buffer* output_buffer) {
    input = input_buffer;
    output = output_buffer;

    initializeBiquads();
}

Amplitude::~Amplitude() {
    delete c1, c2, bq1, bq2, link1, analysis_bus;
    #ifdef MULTICHANNEL
        delete c3, c4, bq3, bq4, link2, link3, link4;
    #endif
}

void Amplitude::initializeBiquads() {
    // TODO: implement linking buffers (cri)
    int len = input->getSize();
    analysis_bus = new Buffer(len);
    link1 = new Buffer(len);
    #ifdef MULTICHANNEL
        link2 = new Buffer(len);
        link3 = new Buffer(len);
        link4 = new Buffer(len);
    #endif

    // Left/mono channel
    c1 = new Coefficients(1.94632, -0.94657, 0.94657, -1.94632, 1.0);
    c2 = new Coefficients(0.83774, -0.06338, 0.06338, -0.83774, 1.0);
    bq1 = new Biquad(1, c1, input, link1);
    bq2 = new Biquad(1, c2, link1, analysis_bus);
    
    // Right channel
    #ifdef MULTICHANNEL
        c3 = new Coefficients(-0.02569, 0.260502, -0.260502, 0.02569, 1.0);
        c4 = new Coefficients(1.8685, -0.870686, 0.870686, -1.8685, 1.0);
        bq3 = new Biquad(1, c3, input, link3);
        bq4 = new Biquad(1, c4, link3, link4);
    #endif
}

std::vector<int>* Amplitude::DetectOnsets() {
    std::vector<int> onsets;
    std::vector<int> avg;

    // Strategy:
    // - Loop over all samples and
    //  - Get the sample
    //  - Run the Hilbert transform
    //  - Compare both signals to get amplitude
    //  - Track 
    for(int i = 0; i < input->getSize(); i++) {
        sample_t sig = input->getSample(i);
        Hilbert();
        sample_t transform = analysis_bus->getSample(i);
        // auto res = transform - sig;
        verbose(abs(transform - sig));
        // Calculate average amplitude
        avg.push_back((analysis_bus->getSample(i) + analysis_bus->getSample(i - 1)) * 0.5);
        // Highpass filter for fast change detection 

        analysis_bus->tick();
    }

    return &onsets;
}

void Amplitude::Hilbert() {
    bq1->process();
    bq2->process();
}