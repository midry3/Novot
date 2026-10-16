#include "Tween.h"
#include <Math/Utils.h>
#include <algorithm>

std::shared_ptr<Tween> TweenGroup::CreateTween(const TweenTarget& target, TweenValue end, double duration, Easing::EasingFunction easing, bool restore) {
	auto t = std::make_shared<Tween>(target, end, duration);
	t->SetEasing(easing);
	if (restore) {
		t->RestoreValue();
	}
	tweens.push_back(t);
	return t;
}

void TweenGroup::Delay(double t) {
	auto twn = std::make_shared<Tween>(&_dummy, 0, t);
	tweens.push_back(twn);
}

bool TweenGroup::IsFinished() const {
	for (const auto& tween : tweens) {
		if (!tween->isFinished) {
			return false;
		}
	}
	return true;
}

Tween* Tween::From(TweenValue start) {
	this->start = start;
	if (std::holds_alternative<Vector2D>(start)) {
		SetTweenTargetValue(target, std::get<Vector2D>(start));
	}
	else {
		SetTweenTargetValue(target, ToTweenDouble(start));
	}
	return this;
}

void Tween::ApplyNoise() {
	if (std::holds_alternative<Vector2D>(noise)) {
		auto n = std::get<Vector2D>(noise);
		if (std::holds_alternative<Vector2D>(rawEnd)) {
			end = std::get<Vector2D>(rawEnd) + GetRandomVector2D(-n, n);
		}
		else {
			end = ToTweenDouble(rawEnd) + GetRandomDouble(-n.Length(), n.Length());
		}
	}
	else {
		auto n = ToTweenDouble(noise);
		if (std::holds_alternative<Vector2D>(rawEnd)) {
			end = std::get<Vector2D>(rawEnd) + GetRandomVector2D(-Vector2D(n, n), Vector2D(n, n));
		}
		else {
			end = ToTweenDouble(rawEnd) + GetRandomDouble(-n, n);
		}
	}
}

void Tween::Update(float dt) {
	if (isFinished) return;
	if (duration <= 0.0) {
		ToEnd();
		isFinished = true;
		Completed.Emit();
		return;
	}
	if (elapsedTime == 0.0) {
		Init();
	}
	elapsedTime += dt;
	double t = (std::min)(1.0, elapsedTime / duration);
	if (std::holds_alternative<Vector2D>(start)) {
		SetTweenTargetValue(target, _startVector + (_endVector - _startVector) * _easing(t));
	}
	else {
		SetTweenTargetValue(target, _startd + (_endd - _startd) * _easing(t));
	}
	if (1.0 <= t) {
		if (std::holds_alternative<Vector2D>(end)) {
			SetTweenTargetValue(target, std::get<Vector2D>(end));
		}
		else {
			SetTweenTargetValue(target, _endd);
		}
		isFinished = true;
		Completed.Emit();
	}
}

void TweenGroup::_Update(float dt) {
	if (_completed || tweens.empty() || loopCount == 0) return;
	tweens[currentTweenIndex]->Update(dt);
	if (tweens[currentTweenIndex]->isFinished) {
		currentTweenIndex++;
		if (tweens.size() <= currentTweenIndex) {
			if (loopCount < 0 || 0 < --loopCount) {
				currentTweenIndex = 0;
				_completed = false;
				for (const auto& tween : tweens) {
					tween->Init();
				}
			}
			else {
				currentTweenIndex = tweens.size() - 1;
				_completed = true;
				Completed.Emit();
			}
		}
	}
}