//
// Created by Daniël Kamp on 18/05/2022.
//

#ifndef DONUT_SCHEDULER_H
#define DONUT_SCHEDULER_H

#include <functional>
#include <vector>
#include <string>

#include <Global.h>
#include <System/ParameterStore.h>

#define TICKS_PER_BEAT 48

using namespace std;

struct TimeSignature {
	int upper;
	int lower;
};

struct Timestamp {
	int measure;
	int beat;
	int tick;
	
	int getInterval() {
		return (measure * beat * TICKS_PER_BEAT) + tick;
	}
};

struct SchedulerEvent {
	string name;
	function<void(void)> callback;
	Timestamp timestamp;
};

class Scheduler {
public:
	Scheduler (Parameter* bpm);
	~Scheduler ();

	void store(function<void(void)>, Timestamp, string);
	void fire();
	uint calculatePosition(Timestamp);
	
	bool remove(const string&);
	uint getSchedulingInterval();
private:
	vector<SchedulerEvent> cb;
	
	TimeSignature signature = {4, 4};
	Parameter* bpm;
	int cycles_per_tick_ = 1;
	int internal_divider_ = 0;
	long position = 0;
	long size;
};


#endif //DONUT_SCHEDULER_H
