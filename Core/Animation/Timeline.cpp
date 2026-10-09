#include "Timeline.h"

void Timeline::TimelineItem::Execute() {
	callback();
}

void Timeline::_Load() {
	Stop();
}

void Timeline::_Update(float dt) {
	if (_timeline.empty()) {
		Completed();
		if (_autoReset) {
			Reset();
		}
		return;
	}
	_currentTime += dt;
	for (auto it = _timeline.begin(); it != _timeline.end(); ++it) {
		if (it->time <= _currentTime) {
			it->Execute();
			it = _timeline.erase(it);
			_finishedItem.push_back(*it);
		}
	}
}

Timeline* Timeline::AddItem(double time, const std::function<void()>& callback) {
	return AddItem({ time, callback });
}

Timeline* Timeline::AddItem(TimelineItem item) {
	for (auto it = _timeline.begin(); it != _timeline.end(); ++it) {
		if (it->time < item.time) {
			_timeline.insert(it, item);
			return this;
		}
	}

	_timeline.push_back(item);
	return this;
}

void Timeline::Reset() {
	_currentTime = 0.0;
	_timeline.splice(_timeline.begin(), _finishedItem);
	_finishedItem.clear();
}