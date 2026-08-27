#include "Tween.h"
#include "MathUtils.h"
#include <algorithm>

Tween* Tween::From(TweenValue start) {
	this->start = start;
	SetTweenTargetValue(target, ToDouble(start));
	return this;
}

void Tween::Update(float dt) {
	if (duration <= 0.0) {
		SetTweenTargetValue(target, ToDouble(end));
		isFinished = true;
		Completed.Emit();
		return;
	}
	elapsedTime += dt;
	double t = (std::min)(1.0, elapsedTime / duration);
	double s = ToDouble(start);
	double e = ToDouble(end);
	SetTweenTargetValue(target, s + (e - s) * t);
	if (t >= 1.0) {
		SetTweenTargetValue(target, e);
		isFinished = true;
		Completed.Emit();
	}
}

void TweenGroup::_Update(float dt) {
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
				Completed.Emit();
			}
		}
	}
}