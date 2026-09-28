#include "AnimatedUI.h"
#include <Animation/Tween.h>

void AnimatedUI::In(bool immediate) {
	KillAllTweenGroups();
	KillAllTimers();
	Enable();
	InStarted();
	const Vector2D& wsize = PropertyManager.GetSettingWindowSize();
	for (const auto& scene : _scenes) {
		const auto& tg = scene->CreateTweenGroup();
		if (immediate) {
			scene->offset = Vector0;
		} else {
			tg->CreateTween(&scene->offset, Vector0, _inTime, _inEasing);
		}
	}
	if (immediate) {
		InFinished();
	}
	else {
		CreateTimer(_inTime, [this]() {
			InFinished();
			});
	}
}

void AnimatedUI::Out(bool immediate) {
	KillAllTweenGroups();
	KillAllTimers();
	OutStarted();
	const Vector2D& wsize = PropertyManager.GetSettingWindowSize();
	for (const auto& scene : _scenes) {
		const auto& tg = scene->CreateTweenGroup();
		const auto& xRate = scene->position.x / wsize.x;
		const auto& yRate = scene->position.y / wsize.y;
		if (yRate < xRate) {
			if (immediate) {
				scene->offset.x = GetNearest(scene->position.x, 0.0, wsize.x) - scene->position.x;
			}
			else {
				tg->CreateTween(&scene->offset.x, GetNearest(scene->position.x, 0.0, wsize.x) - scene->position.x, _outTime, _outEasing);
			}
		}
		else {
			if (immediate) {
				scene->offset.y = GetNearest(scene->position.y, 0.0, wsize.y) - scene->position.y;
			}
			else {
				tg->CreateTween(&scene->offset.y, GetNearest(scene->position.y, 0.0, wsize.y) - scene->position.y, _outTime, _outEasing);
			}
		}
	}
	if (immediate) {
		Disable();
		OutFinished();
	}
	else {
		CreateTimer(_outTime, [this]() {
			Disable();
			OutFinished();
			});
	}
}