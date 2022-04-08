//
// Created by Daniël Kamp on 20/03/2022.
//

#include <Source/Tensions.h>

Tensions::Tensions(Tables* tables, ParameterPool* params, uint8_t voice_id) : Source(params, voice_id) {
	ks_delaytime = parameters->get(p_KS_DelayTime, voice_id);
	ks_feedback = parameters->get(p_KS_Feedback, voice_id);
	ks_dampening = parameters->get(p_KS_FilterCutoff, voice_id);
	
	this->transpose = parameters->get(p_KS_Transpose, voice_id);
	
	dl_in = new Buffer(samplerate/4.0);
	delayLine = new FilterDelayLine(ks_delaytime, ks_feedback, ks_dampening, dl_in, output);
	
	exciter = new Exciter {tables, dl_in};
	
	remaining_trigger_time = 0;
	trigger_time = 10;
}

Tensions::~Tensions() {
	delete dl_in;
	delete delayLine;
}

void Tensions::pitch(uint8_t note) {
	ks_delaytime->value = mtof(note + (uint8_t) transpose->value, 440.0);
	
	// Handle excitation via an interface struct
	exciter->excite("pulse", trigger_time);
	remaining_trigger_time = trigger_time;
}

void Tensions::process() {
	// Do the actual delay line processing
	delayLine->process();
	
	// Push the DL and buffer forward
	delayLine->tick();
	dl_in->tick();
	
	// Write a 0 for the next sample (if a burst isn't in progress)
	if(remaining_trigger_time > 0) {
		remaining_trigger_time--;
	} else {
		dl_in->flush();
	}
}

void Tensions::refresh () {}