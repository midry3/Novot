#pragma once
#include <Scene.h>
#include <Collision/Collision.h>

class RayCast : public Collision {
protected:
	Vector2D _nearest;
	void _Load() override;
	void _Update(float dt) override;
public:
	RayCast* SetStartPoint(const Vector2D& start) {
		ChangeVertice(0, start);
		return this;
	}
	RayCast* SetEndPoint(const Vector2D& end) {
		ChangeVertice(1, end);
		return this;
	}
	RayCast* SetPoint(const Vector2D& start, const Vector2D& end) {
		SetStartPoint(start);
		SetEndPoint(end);
		return this;
	}
	Vector2D GetCollidePoint() {
		return _nearest;
	}
	Vector2D GetCollideLocalPoint() {
		return _nearest - GetGlobalPosition();
	}
};