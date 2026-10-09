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

	tg->CreateTween(&position, Vector0, 0.1);
}