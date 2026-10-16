#include "SpationalSoundPlayer2D.h"

void SpationalSoundPlayer2D::_Update(float dt) {
	if (auto ptr = _trackingTarget.lock()) {
		SetVolume(static_cast<float>(1.0 / Max(1.0, (GetGlobalPosition() - ptr->GetGlobalPosition()).Length())));
	}
}

SpationalSoundPlayer2D* SpationalSoundPlayer2D::SetTracking(WeakPtr<Scene> scene) {
	_trackingTarget = scene;
	return this;
}