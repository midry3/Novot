#include "Timer.h"
#include "NovotCore.h"

std::shared_ptr<Timer> _TimerManager::New(double time, std::function<void()> f, bool loop) {
	auto timer = std::make_shared<Timer>(time, f, loop);
	timers.push_back(timer);
	return timer;
}

void _TimerManager::Update(float dt) {
	for (auto it = timers.begin(); it != timers.end();) {
		(*it)->Update(dt);
		if ((*it)->isFinished) {
			it = timers.erase(it);
		}
		else {
			++it;
		}
	}
}