#pragma once

class StopWatch {
private:
	int _startTime = 0;
	bool _isRunning = false;
public:
	StopWatch() = default;

	void Restart();
	int Measure();
};