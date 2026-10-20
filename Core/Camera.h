#pragma once
#include <Signal.h>
#include <SceneManager.h>
#include <Scene.h>
#include <Singleton.h>
#include <Math/Vector2D.h>
#include <Asset/Texture/Texture.h>
#include <memory>

class _Camera : public Singleton<_Camera> {
protected:
	Vector2D _minimums = Vector2D(-9999999, -9999999);
	Vector2D _maximums = Vector2D(9999999, 9999999);
	Vector2D _zoom = Vector2D(1, 1);
	std::weak_ptr<Scene> target;
public:
	Signal<Vector2D, Vector2D> ZoomChanged;
	Vector2D position = Vec2D_0;
	void Shake(int num, double intensity);
	void SetPosition(const Vector2D& position) {
		this->position = position;
	}
	Vector2D GetPosition() const {
		return position;
	}
	Vector2D GetZoom() const {
		return _zoom;
	}
	_Camera& SetZoom(const Vector2D& zoom);
	_Camera& SetZoom(double zoom) {
		return SetZoom(Vector2D(zoom, zoom));
	}
	_Camera& SetZoom(double x, double y) {
		return SetZoom(Vector2D(x, y));
	}
	Vector2D WorldToScreen(const Vector2D& worldPos) const;
	Vector2D ScreenToWorld(const Vector2D& screenPos) const;
	_Camera& SetMinimums(const Vector2D& minimums) {
		_minimums = minimums;
		return *this;
	}
	_Camera& SetMinimumX(double x) {
		_minimums.x = x;
		return *this;
	}
	_Camera& SetMinimumY(double y) {
		_minimums.y = y;
		return *this;
	}
	_Camera& SetMaximums(const Vector2D& maximums) {
		_maximums = maximums;
		return *this;
	}
	_Camera& SetMaximumX(double x) {
		_maximums.x = x;
		return *this;
	}
	_Camera& SetMaximumY(double y) {
		_maximums.y = y;
		return *this;
	}
	_Camera& FitTo(const std::shared_ptr<Scene>& scene);
	Vector2D GetPoint() const {
		auto pos = position + GetTargetPoint();
		pos.x = std::clamp(pos.x, _minimums.x, _maximums.x);
		pos.y = std::clamp(pos.y, _minimums.y, _maximums.y);
		return pos;
	}
	_Camera& SetTracking(std::weak_ptr<Scene> target) {
		this->target = target;
		return *this;
	}
	Vector2D GetTargetPoint() const {
		if (auto scene = target.lock()) {
			return scene->GetGlobalPosition();
		}
		return Vec2D_0;
	}
};

#define Camera _Camera::GetInstance()