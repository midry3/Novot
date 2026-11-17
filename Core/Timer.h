#pragma once
#include "Singleton.h"
#include <LogicScene.h>
#include <functional>
#include <list>
#include <memory>

class Timer : public LogicScene {
protected:
	int loopNum;
	double time, startTime;
	bool isPaused = false;
	void _Update(float dt) override {
		Update(dt);
	}
public:
	Signal<> Timeout;
	Signal<> Completed;
	bool isFinished = false;
	Timer() = default;
	Timer(double time, const std::function<void()>& callback, int loopNum=1) : time(time), startTime(time), loopNum(loopNum) {
		Timeout.Connect(callback);
		Hide();
	}
	void Update(float dt) {
		if (isPaused || isFinished) return;
		time -= dt;
		if (time <= 0) {
			if (loopNum < 0) {
				Timeout.Emit();
				time = startTime;
			}
			else if (loopNum > 0) {
				Timeout.Emit();
				time = startTime;
				loopNum--;
			}
			else {
				isFinished = true;
				Completed.Emit();
			}
		}
	}
	void Kill() {
		time = std::numeric_limits<double>::infinity();
		isFinished = true;
		Free();
	}
	Timer* SetTime(double time) {
		this->time = time;
		this->startTime = time;
		return this;
	}
	Timer* SetLoop(int loopNum) {
		this->loopNum = loopNum;
		return this;
	}
	Timer* OnTimeout(const std::function<void()>& callback) {
		Timeout.Connect(callback);
		return this;
	}
	Timer* OnCompleted(const std::function<void()>& callback) {
		Completed.Connect(callback);
		return this;
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
