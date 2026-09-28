#pragma once
#include <Scene.h>
#include <Animation/EasingFunction.h>

class AnimatedUI : public Scene {
protected:
	double _inTime = 0.5, _outTime = 0.5;
	Easing::EasingFunction _inEasing = Easing::EaseInQuad, _outEasing = Easing::EaseInQuad;
public:
	Signal<> InStarted, InFinished, OutStarted, OutFinished;
	void In(bool immediate = false);
	void Out(bool immediate = false);
	AnimatedUI* SetInTime(double time) {
		_inTime = time;
		return this;
	}
	AnimatedUI* SetOutTime(double time) {
		_outTime = time;
		return this;
	}
};