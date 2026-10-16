#pragma once
#include "SoundPlayer.h"

class SpationalSoundPlayer2D : public SoundPlayer {
protected:
	WeakPtr<Scene> _trackingTarget;

	void _Update(float dt) override;
public:
	SpationalSoundPlayer2D* SetTracking(WeakPtr<Scene> scene);
};