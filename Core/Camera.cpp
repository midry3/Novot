#include "Camera.h"

void _Camera::Shake(int num, double intensity) {
	const auto& tg = SceneManager.GetCurrentScene()->CreateTweenGroup();

	for (int i=0; i<num; i++) {
		if (i % 2 == 0) {
			tg->CreateTween(&position, GetRandomIntVector2D(Vector2D(-intensity, 0), Vector2D(intensity, intensity)), 0.1);
		}
		else {
			tg->CreateTween(&position, GetRandomIntVector2D(Vector2D(-intensity, -intensity), Vector2D(intensity, 0)), 0.1);
		}
	}

	tg->CreateTween(&position, Vec2D_0, 0.1);
}

_Camera& _Camera::FitTo(const std::shared_ptr<Scene>& scene) {
	if (scene) {
		SetMinimums(scene->GetGlobalPosition());
		SetMaximums(scene->GetGlobalPosition() + scene->GetSize() - PropertyManager.GetSettingWindowSize());
	}
	return *this;
}