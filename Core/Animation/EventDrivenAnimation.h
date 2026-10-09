#pragma once
#include "Timeline.h"

class EventDrivenAnimation : public Scene {
protected:
	void _Load() override;
	void _Update(float dt) override;

	std::vector<std::shared_ptr<Timeline>> _timelines;
public:
	std::shared_ptr<Timeline> AddAnimation(const int id);

	void Play(const int id);
	void Stop(const int id);
};