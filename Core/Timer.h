#pragma once
#include "Singleton.h"
#include <functional>
#include <list>
#include <memory>

class Timer {
private:
	double time, startTime;
	bool isPaused = false;
	bool isLooped = false;
	std::function<void()> callback;
public:
	bool isFinished = false;
	Timer(double time, std::function<void()> callback, bool loop=false) : time(time), startTime(time), callback(callback), isLooped(loop) {}
	void Update(float dt) {
		if (isPaused || isFinished) return;
		time -= dt;
		if (time <= 0) {
			callback();
			if (isLooped) {
				time = startTime;
			}
			else {
				isFinished = true;
			}
		}
	}
	void Kill() {
		time = std::numeric_limits<double>::infinity();
		isFinished = true;
	}
	Timer& OnComplete(std::function<void()> callback) {
		this->callback = callback;
		return *this;
	}
	void Pause() {
		isPaused = true;
	}
	void Resume() {
		isPaused = false;
	}
};

class _TimerManager : public Singleton<_TimerManager> {
private:
	std::list<std::shared_ptr<Timer>> timers;
public:
	_TimerManager() = default;
	~_TimerManager() = default;
	std::shared_ptr<Timer> New(double time, std::function<void()> f, bool loop=false);
	void Update(float dt);
};

#define TimerManager _TimerManager::GetInstance()
