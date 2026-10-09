#pragma once
#include "Tween.h"

class Timeline : public Scene {
protected:
	struct TimelineItem {
		double time;
		std::function<void()> callback;
		
		void Execute();
	};

	double _currentTime = 0.0;
	bool _autoReset = false;
	std::list<TimelineItem> _timeline, _finishedItem;

	void _Load() override;
	void _Update(float dt) override;
public:
	Signal<> Completed;

	Timeline* AddItem(TimelineItem item);
	Timeline* AddItem(double time, const std::function<void()>& callback);
	Timeline* AutoReset() {
		_autoReset = true;
		return this;
	}

	void Start() {
		Enable();
	}

	void Stop() {
		Disable();
	}

	void Reset();
};