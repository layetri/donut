//
// Created by Daniël Kamp on 19/05/2022.
//

#ifndef DONUT_RANDOMGENERATOR_H
#define DONUT_RANDOMGENERATOR_H

#include <Global.h>
#include <Modulator/Modulator.h>

class RandomGenerator : public Modulator {
public:
	RandomGenerator (Parameter*, Parameter*, Parameter*, Parameter*, ModID, string, uint8_t);
	~RandomGenerator ();

	void process() override;
	void syncToScheduler() override;
private:
	Parameter *rate, *range, *bpm_sync, *slew;
	uint interval = 0, counter = 0;
	float n = 1.0, n_1 = 1.0;
};


#endif //DONUT_RANDOMGENERATOR_H
