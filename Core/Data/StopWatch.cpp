#include "StopWatch.h"
#include <DxLib.h>

void StopWatch::Restart() {
	_startTime = GetTickCount();
	_isRunning = true;
}

int StopWatch::Measure() {
	if (_isRunning) {
		return GetTickCount() - _startTime;
	}
	return 0;
}