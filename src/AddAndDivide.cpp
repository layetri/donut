//
// Created by Daniël Kamp on 17/11/2021.
//

#include <DSP/AddAndDivide.h>

AddAndDivide::AddAndDivide(vector<Buffer*>* inputs, ParameterPool* params, uint8_t voice_id, Buffer* output) {
  	this->inputs = inputs;
  	this->output = output;
  	this->filter_queue = new Buffer(output->getSize());
  	output_filter = new HighPassFilter(params->get(p_OutputHPF_Frequency, voice_id), filter_queue, output);
	  
	this->parameters = params;
	ws1 = params->get(p_WS1_Amount, voice_id);
	ws2 = params->get(p_WS2_Amount, voice_id);
	wt1 = params->get(p_WT1_Amount, voice_id);
	wt2 = params->get(p_WT2_Amount, voice_id);
	ks = params->get(p_KS_Amount, voice_id);
	smp = params->get(p_Sampler_Amount, voice_id);
	part = params->get(p_Particles_Amount, voice_id);
	master = params->get(p_VoiceMaster, voice_id);
}

AddAndDivide::~AddAndDivide() {
	delete output_filter;
	delete filter_queue;
}

void AddAndDivide::process() {
  long val = 0;
  auto div = ws1->value + ws2->value + wt1->value + wt2->value + ks->value + smp->value + part->value;

  for(auto& b : *inputs) {
    val += b->getCurrentSampleMultiplied();
  }
  div = div < 1.0f ? 1.0f : div;
//  val = (sample_t) ((float) val / ((float) (div == 0.0f) + div));
  val = (sample_t) ((float) val / 7.0f);

  filter_queue->write(val * master->value);
  output_filter->process();
}

void AddAndDivide::tick() {
	filter_queue->tick();
	output_filter->tick();
}