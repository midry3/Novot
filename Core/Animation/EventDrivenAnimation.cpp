#include "EventDrivenAnimation.h"

std::shared_ptr<Timeline> EventDrivenAnimation::AddAnimation(const int id) {
	auto timeline = CreateScene<Timeline>();
	timeline->AutoReset();
	if (_timelines.size() <= id) {
		_timelines.resize(id + 1);
	}
	_timelines[id] = timeline;
	return timeline;
}

void EventDrivenAnimation::Play(const int id) {
	if (id < 0 || id >= _timelines.size()) {
		return;
	}
	if (const auto& target = _timelines[id]) {
		target->Start();
	}
}

void EventDrivenAnimation::Stop(const int id) {
	if (id < 0 || id >= _timelines.size()) {
		return;
	}
	if (const auto& target = _timelines[id]) {
		target->Stop();
	}
}