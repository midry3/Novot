#pragma once
#include "EasingFunction.h"
#include <Math/Utils.h>
#include <Event/Signal.h>
#include <Scene.h>
#include <Math/Vector2D.h>
#include <functional>
#include <variant>
#include <memory>
#include <vector>
#include <type_traits>
#include <cmath>

using TweenTarget = std::variant<double*, float*, int*, Vector2D*>;
using TweenValue = std::variant<double, float, int, Vector2D>;

namespace {
	int _dummy=0;
}

inline double ToTweenDouble(const TweenValue& value) {
	return std::visit([](const auto& v) -> double {
		using T = std::decay_t<decltype(v)>;
		if constexpr (std::is_same_v<T, Vector2D>) {
			return 0.0;
		}
		else {
			return static_cast<double>(v);
		}
	}, value);
}

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
		if constexpr (std::is_same_v<T, Vector2D>) {
			return;
		}
		else if constexpr (std::is_integral_v<T>) {
			*p = static_cast<T>(std::lround(value));
		}
		else {
			*p = static_cast<T>(value);
		}
	}, target);
}

inline void SetTweenTargetValue(const TweenTarget& target, const Vector2D& value) {
	std::visit([&value](auto* p) {
		if (p == nullptr) return;
		using T = std::remove_pointer_t<decltype(p)>;
		if constexpr (std::is_same_v<T, Vector2D>) {
			*p = value;
		}
	}, target);
}

class Tween {
private:
	Easing::EasingFunction _easing = Easing::Linear;
	double duration, elapsedTime, _startd=0.0, _endd=0.0;
	Vector2D _startVector, _endVector;
	TweenValue start, end, rawEnd, noise=0.0;
	TweenTarget target;
protected:
	void ToEnd() {
		if (std::holds_alternative<Vector2D>(end)) {
			SetTweenTargetValue(target, std::get<Vector2D>(end));
		}
		else {
			SetTweenTargetValue(target, ToTweenDouble(end));
		}
	}
public:
	Signal<> Completed;
	bool isFinished = false;
	Tween() = default;
	Tween(const TweenTarget& target, TweenValue end, double duration) : duration(duration), elapsedTime(0), start(GetTweenTargetValue(target)), end(end), rawEnd(end), target(target) {}
	~Tween() = default;
	Tween* From(TweenValue start);
	Tween* OnComplete(const std::function<void()>& callback) {
		Completed.Connect(callback);
		return this;
	}
	Tween* SetEasing(const Easing::EasingFunction& easing) {
		_easing = easing;
		return this;
	}
	Tween* RestoreValue() {
		Completed += [this]() {
			if (std::holds_alternative<Vector2D>(start)) {
				SetTweenTargetValue(target, std::get<Vector2D>(start));
			}
			else {
				SetTweenTargetValue(target, ToTweenDouble(start));
			}
			};	
		return this;
	}
	Tween* SetNoise(TweenValue noise) {
		this->noise = noise;
		return this;
	}
	void ApplyNoise();
	void Update(float dt);
	void Kill() {
		elapsedTime = 0.0;
		isFinished = true;
	}
	void Skip() {
		Kill();
		ToEnd();
		Completed();
	}
	void Init() {
		elapsedTime = 0.0;
		isFinished = false;
		start = GetTweenTargetValue(target);
		ApplyNoise();
		if (std::holds_alternative<Vector2D>(start)) {
			_startVector = std::get<Vector2D>(start);
			_endVector = std::get<Vector2D>(end);
		}
		else {
			_startd = ToTweenDouble(start);
			_endd = ToTweenDouble(end);
		}
	}
};

class TweenGroup : public Scene {
protected:
	std::vector<std::shared_ptr<Tween>> tweens;
	int loopCount = 1;
	int currentTweenIndex = 0;
	bool _completed = false;
	void _Update(float dt) override;
public:
	Signal<> Completed;
	TweenGroup() {
		Hide();
	}
	~TweenGroup() = default;
	std::shared_ptr<Tween> CreateTween(const TweenTarget& target, TweenValue end, double duration, Easing::EasingFunction easing = Easing::Linear, bool restore = false);
	void Add(std::shared_ptr<Tween> tween) {
		tweens.push_back(tween);
	}
	void Delay(double t);
	bool IsFinished() const;
	void SetLoop(int loop) {
		loopCount = loop;
	}
	void SetLoop() {
		loopCount = -1;
	}
	void OnComplete(const std::function<void()>& callback) {
		Completed.Connect(callback);
	}
	void Kill() {
		for (const auto& tween : tweens) {
			tween->Kill();
		}
		Free();
	}
	void Skip() {
		for (const auto& tween : tweens) {
			tween->Skip();
		}
		Free();
	}
};