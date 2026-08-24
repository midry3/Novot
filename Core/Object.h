#pragma once
#include "Collision/Collision.h"
#include "Collision/CollisionManager.h"
#include "Scene.h"
#include "MathUtils.h"

class Object : public Scene {
protected:
	virtual void _Load() override;
	virtual void _Release() override;
	virtual void _Update(float dt) override;
	virtual void _Draw() const override;
	Vector2D prePosition, preGlobalPosition;
public:
	std::shared_ptr<Collision> collision;
	Vector2D scale, velocity, acceleration;
	double rotation = 0.0;
	Object() : scale(1, 1), velocity(0, 0), acceleration(0, 0) {
		collision = CollisionManager.New();
		AddScene(collision);
	}
	void MoveAndSlide(float dt);
	void SetRotationDegree(double degree) {
		rotation = degree * 3.14159265358979323846 / 180.0;
	}
	void SetRotationRadian(double radian) {
		rotation = radian;
	}
	void SetRotationTurn(double turn) {
		rotation = turn * 3.14159265358979323846 * 2.0;
	}
	void SetScale(const Vector2D& s) {
		scale = s;
	}
	void SetAcceleration(const Vector2D& a) {
		acceleration = a;
	}
};