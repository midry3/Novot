#pragma once
#include <Signal.h>
#include <SceneManager.h>
#include <Scene.h>
#include <Singleton.h>
#include <Math/Vector2D.h>
#include <memory>

class _Camera : public Singleton<_Camera> {
protected:
	Vector2D _minimums = Vector2D(-9999999, -9999999);
	Vector2D _maximums = Vector2D(9999999, 9999999);
	Vector2D _zoom = Vector2D(1, 1);
	std::weak_ptr<Scene> target;
public:
	Signal<Vector2D, Vector2D> ZoomChanged;
	Vector2D position = Vector0;
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
	_Camera& SetZoom(const Vector2D& zoom) {
		ZoomChanged.Emit(_zoom, zoom);
		_zoom = zoom;
		return *this;
	}
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
		return Vector0;
	}
};

#define Camera _Camera::GetInstance()