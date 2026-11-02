#pragma once
#include "Sprite2D.h"

class ProgressSprite2D : public Sprite2D {
protected:
	double _progress = 0.0, _actualProgress = 0.0, _animationTime = 1.0;
	void _Draw() const override;
public:
	ProgressSprite2D* SetProgress(double progress);
	ProgressSprite2D* SetProgressAnimated(double progress);
	ProgressSprite2D* SetAnimationTime(double time) {
		_animationTime = time;
		return this;
	}
	const Vector2D& GetRawSize() const override {
		return Vector2D(_size.x * _actualProgress, _size.y);
	}
	[[nodiscard]] double GetProgress() const {
		return _actualProgress;
	}
	void AdvanceProgress(double delta) {
		SetProgress(_actualProgress + delta);
	}
	void AdvanceProgressAnimated(double delta) {
		SetProgressAnimated(_actualProgress + delta);
	}
};