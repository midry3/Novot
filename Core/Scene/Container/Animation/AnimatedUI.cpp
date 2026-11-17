#include "AnimatedUI.h"
#include <Animation/Tween.h>

void AnimatedUI::In(bool immediate) {
	KillAllTweenGroups();
	KillAllTimers();
	Enable();
	InStarted();
	const Vector2D& wsize = PropertyManager.GetSettingWindowSize();
	for (const auto& scene : GetVisualScenes()) {
		scene->UpdateEneble();
		const auto& tg = scene->CreateTweenGroup();
		if (immediate) {
			scene->offset = Vec2D_0;
		} else {
			tg->CreateTween(&scene->offset, Vec2D_0, _inTime, _inEasing);
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
	for (const auto& scene : GetVisualScenes()) {
		scene->UpdateDisable();
		const auto& tg = CreateTweenGroup();
		const auto& gpos = scene->GetGlobalPosition();
		const auto& dx = GetNearest(gpos.x, 0.0, wsize.x) - gpos.x - scene->GetSize().x;
		const auto& dy = GetNearest(gpos.y, 0.0, wsize.y) - gpos.y - scene->GetSize().y;
		if (dx < dy) {
			if (immediate) {
				scene->offset.x = dx;
			}
			else {
				tg->CreateTween(&scene->offset.x, dx, _outTime, _outEasing);
			}
		}
		else {
			if (immediate) {
				scene->offset.y = dy;
			}
			else {
				tg->CreateTween(&scene->offset.y, dy, _outTime, _outEasing);
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