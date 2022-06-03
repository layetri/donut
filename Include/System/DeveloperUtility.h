//
// Created by Daniël Kamp on 03/06/2022.
//

#ifndef DONUT_DEVELOPERUTILITY_H
#define DONUT_DEVELOPERUTILITY_H

#include <Global.h>

#include <algorithm>
#include <numeric>
#include <deque>

struct StatsContainer {
	uint avgJackCycleTime = 0;
	uint avgProcessTime = 0;
	uint maxProcessTime = 0;
	uint blockSize = 0;
	uint allowedCycleTime = 0;
	uint sampleRate = samplerate;
};

class DeveloperUtility {
public:
	DeveloperUtility ();
	~DeveloperUtility ();

	StatsContainer getMeasurements();
	
	void registerJackCycleTimeMeasurement(uint measurement);
	void registerProcessingTimeMeasurement(uint measurement);
	void registerBlockSize(uint block_size);
private:
	uint max_sample_size = 100;
	std::deque<uint> jack_cycle_time_samples;
	std::deque<uint> process_time_samples;
	uint block_size = 0;
};


#endif //DONUT_DEVELOPERUTILITY_H
