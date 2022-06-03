//
// Created by Daniël Kamp on 03/06/2022.
//

#include "System/DeveloperUtility.h"

DeveloperUtility::DeveloperUtility () {

}

DeveloperUtility::~DeveloperUtility () {

}

StatsContainer DeveloperUtility::getMeasurements () {
	auto const count_j = static_cast<float>(jack_cycle_time_samples.size());
	uint jack_avg = std::reduce(jack_cycle_time_samples.begin(), jack_cycle_time_samples.end()) / count_j;
	
	auto const count_p = static_cast<float>(process_time_samples.size());
	uint process_avg = std::reduce(process_time_samples.begin(), process_time_samples.end()) / count_p;
	uint process_max = *std::max_element(process_time_samples.begin(), process_time_samples.end());
	
	uint allowed = (uint) ((1000000.0f / (float) samplerate) * block_size);
	
	return StatsContainer {jack_avg, process_avg, process_max, block_size, allowed};
}

void DeveloperUtility::registerJackCycleTimeMeasurement (uint measurement) {
	jack_cycle_time_samples.push_back(measurement);
	if(jack_cycle_time_samples.size() > max_sample_size) {
		jack_cycle_time_samples.pop_front();
	}
}

void DeveloperUtility::registerProcessingTimeMeasurement (uint measurement) {
	process_time_samples.push_back(measurement);
	if(process_time_samples.size() > max_sample_size) {
		process_time_samples.pop_front();
	}
}

void DeveloperUtility::registerBlockSize (uint block_size) {
	this->block_size = block_size;
}