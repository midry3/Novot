#include "StopWatch.h"
#include <Backend/Backend.h>

void StopWatch::Restart() {
	_startTime = CoreBackend::Clock::Tick();
	_isRunning = true;
}

int StopWatch::Measure() {
	if (_isRunning) {
		return CoreBackend::Clock::Tick() - _startTime;
	}
	return 0;
}