#pragma once
#include <Scene.h>
#include <functional>
#include <variant>
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
public:
	Signal<> Completed;
	bool isFinished = false;
	Tween() = default;
	Tween(const TweenTarget& target, TweenValue end, double duration) : duration(duration), elapsedTime(0), start(GetTweenTargetValue(target)), end(end), target(target) {}
	~Tween() = default;
	Tween* From(TweenValue start);
	Tween* OnComplete(const std::function<void()>& callback) {
		Completed.Connect(callback);
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

class TweenGroup : public Scene {
protected:
	std::vector<std::shared_ptr<Tween>> tweens;
	int loopCount = 0;
	int currentTweenIndex = 0;
	void _Update(float dt) override;
public:
	Signal<> Completed;
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
	void OnComplete(const std::function<void()>& callback) {
		Completed.Connect(callback);
	}
};