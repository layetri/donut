//
// Created by Daniël Kamp on 17/11/2021.
//

#include <DSP/AddAndDivide.h>

AddAndDivide::AddAndDivide(vector<Buffer*>* inputs, ParameterPool* params, uint8_t voice_id, Buffer* output) {
  	this->inputs = inputs;
  	this->output = output;
	  
	this->parameters = params;
	ws1 = params->get(p_WS1_Amount, voice_id);
	ws2 = params->get(p_WS2_Amount, voice_id);
	wt1 = params->get(p_WT1_Amount, voice_id);
	wt2 = params->get(p_WT2_Amount, voice_id);
	ks = params->get(p_KS_Amount, voice_id);
	master = params->get(p_VoiceMaster, voice_id);
}

AddAndDivide::~AddAndDivide() {}

void AddAndDivide::process() {
  sample_t val = 0;
  auto div = ws1->value + ws2->value + wt1->value + wt2->value + ks->value;

  for(auto& b : *inputs) {
    val += (b->getCurrentSampleMultiplied() / ((div == 0) + div));
  }

  if(master->value > 0.00) {
	  output->write(val * master->value);
  }
}