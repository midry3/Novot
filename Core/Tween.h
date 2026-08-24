#pragma once
#include "Singleton.h"
#include <functional>
#include <variant>
#include <list>
#include <memory>
#include <vector>
#include <type_traits>
#include <cmath>

using TweenTarget = std::variant<double*, float*, int*>;
using TweenValue = std::variant<double, float, int>;

inline TweenValue GetTweenTargetValue(const TweenTarget& target) {
	return std::visit([](auto* p) -> TweenValue {
		if (p == nullptr) return 0;
		using T = std::remove_pointer_t<decltype(p)>;
		return static_cast<T>(*p);
	}, target);
}

inline void SetTweenTargetValue(const TweenTarget& target, double value) {
	std::visit([value](auto* p) {
		if (p == nullptr) return;
		using T = std::remove_pointer_t<decltype(p)>;
		if constexpr (std::is_integral_v<T>) {
			*p = static_cast<T>(std::lround(value));
		}
		else {
			*p = static_cast<T>(value);
		}
	}, target);
}

class Tween {
private:
	double duration, elapsedTime;
	TweenValue start, end;
	TweenTarget target;
	std::function<void()> callback;
public:
	bool isFinished = false;
	Tween() = default;
	Tween(const TweenTarget& target, TweenValue end, double duration) : duration(duration), elapsedTime(0), start(GetTweenTargetValue(target)), end(end), target(target) {}
	~Tween() = default;
	Tween* From(TweenValue start);
	Tween* OnComplete(std::function<void()> callback) {
		this->callback = callback;
		return this;
	}
	void Update(float dt);
	void Kill() {
		elapsedTime = 0.0;
		isFinished = true;
	}
	void Init() {
		elapsedTime = 0.0;
		isFinished = false;
		start = GetTweenTargetValue(target);
	}
};

class TweenGroup {
private:
	std::vector<std::shared_ptr<Tween>> tweens;
	int loopCount = 0;
	int currentTweenIndex = 0;
	std::function<void()> callback;
public:
	TweenGroup() = default;
	~TweenGroup() = default;
	std::shared_ptr<Tween> CreateTween(const TweenTarget& target, TweenValue end, double duration) {
		auto t = std::make_shared<Tween>(target, end, duration);
		tweens.push_back(t);
		return t;
	}
	void Add(std::shared_ptr<Tween> tween) {
		tweens.push_back(tween);
	}
	void Update(float dt) {
		if (IsFinished() || tweens.empty()) return;
		tweens[currentTweenIndex]->Update(dt);
		if (tweens[currentTweenIndex]->isFinished) {
			currentTweenIndex++;
			if (currentTweenIndex >= tweens.size()) {
				if (--loopCount > 0) {
					currentTweenIndex = 0;
					for (const auto& tween : tweens) {
						tween->Kill();
						tween->Init();
					}
				}
				else {
					currentTweenIndex = tweens.size() - 1;
					if (callback) {
						callback();
					}
				}
			}
		}
	}
	bool IsFinished() const {
		for (const auto& tween : tweens) {
			if (!tween->isFinished) {
				return false;
			}
		}
		return true;
	}
	void SetLoop(int loop) {
		loopCount = loop;
	}
	void SetLoop() {
		loopCount = std::numeric_limits<double>::infinity();
	}
	void OnComplete(std::function<void()> callback) {
		this->callback = callback;
	}
};

class _TweenManager : public Singleton<_TweenManager> {
private:
	std::list<std::shared_ptr<TweenGroup>> tweens;
public:
	_TweenManager() = default;
	~_TweenManager() = default;
	void Update(float dt) {
		for (auto it = tweens.begin(); it != tweens.end();) {
			(*it)->Update(dt);
			if ((*it)->IsFinished()) {
				it = tweens.erase(it);
			}
			else {
				++it;
			}
		}
	}

	std::shared_ptr<TweenGroup> CreateGroup() {
		auto t = std::make_shared<TweenGroup>();
		tweens.push_back(t);
		return t;
	}
};
inline _TweenManager& TweenManager() {
	return _TweenManager::GetInstance();
}

#define TweenManager _TweenManager::GetInstance()