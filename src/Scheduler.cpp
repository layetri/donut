//
// Created by Daniël Kamp on 18/05/2022.
//

#include <System/Scheduler.h>

#include <utility>

Scheduler::Scheduler (Parameter* bpm) {
	this->bpm = bpm;
}

Scheduler::~Scheduler () {

}

void Scheduler::store(function<void(void)> callback, Timestamp timestamp, string name) {
	cb.push_back(SchedulerEvent {std::move(name), std::move(callback), timestamp});
}

bool Scheduler::remove(const string& name) {
	for(int i = 0; i < cb.size(); i++) {
		if(cb[i].name == name) {
			cb.erase(cb.begin() + i);
			return true;
		}
	}
	return false;
}

void Scheduler::fire() {
	for(auto& e : cb) {
		if(!(position % calculatePosition(e.timestamp))) {
			e.callback();
		}
	}
	
	position++;
	if(position > (16 * signature.upper * TICKS_PER_BEAT)) {
		position = 0;
	}
}

uint Scheduler::getSchedulingInterval() {
	return (uint) floor(60000000.0f / (float) (bpm->value * TICKS_PER_BEAT));
}

uint Scheduler::calculatePosition(Timestamp t) {
	return ((t.measure * signature.upper) + t.beat) * TICKS_PER_BEAT + t.tick;
}