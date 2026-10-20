#include "Camera.h"
#include <PropertyManager.h>

_Camera& _Camera::SetZoom(const Vector2D& zoom) {
	const Vector2D oldZoom = _zoom;
	_zoom = zoom;
	ZoomChanged.Emit(oldZoom, zoom);
	return *this;
}

Vector2D _Camera::WorldToScreen(const Vector2D& worldPos) const {
	const Vector2D halfScreen = PropertyManager.GetSettingWindowSize() / 2.0;
	return (worldPos - GetPoint() - halfScreen) * _zoom + halfScreen;
}

Vector2D _Camera::ScreenToWorld(const Vector2D& screenPos) const {
	const Vector2D halfScreen = PropertyManager.GetSettingWindowSize() / 2.0;
	const double zx = (_zoom.x == 0.0) ? 1.0 : _zoom.x;
	const double zy = (_zoom.y == 0.0) ? 1.0 : _zoom.y;
	return Vector2D((screenPos.x - halfScreen.x) / zx, (screenPos.y - halfScreen.y) / zy) + halfScreen + GetPoint();
}

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
		const double zx = (_zoom.x == 0.0) ? 1.0 : _zoom.x;
		const double zy = (_zoom.y == 0.0) ? 1.0 : _zoom.y;
		const Vector2D viewSize = Vector2D(
			PropertyManager.GetSettingWindowSize().x / zx,
			PropertyManager.GetSettingWindowSize().y / zy
		);
		SetMinimums(scene->GetGlobalPosition());
		SetMaximums(scene->GetGlobalPosition() + scene->GetSize() - viewSize);
	}
	return *this;
}