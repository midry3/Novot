#pragma once
#include "Collision/Collision.h"
#include "Scene.h"
#include <Math/Vector2D.h>

struct SimulationResult {
	Vector2D position, velocity, acceleration, scale, scaleVelocity;
};

struct MoveAndSlideOption {
	bool positionX = true, positionY = true, velocityX = true, velocityY = true, accelerationX = true, accelerationY = true, scaleX = true, scaleY = true, scaleVelocityX = true, scaleVelocityY = true;
};

class Object2D : public Scene {
protected:
	Vector2D prePosition, preGlobalPosition;
	std::unique_ptr<SimulationResult> _simulationResult;
	std::shared_ptr<Collision> CreateCollision();
	bool isColliding = false;
	int _maxClimbHeight = 5, _maxSlideWidth = 15;

	void _Load() override;
	void _Update(float dt) override;
public:
	std::shared_ptr<Collision> collision;
	Vector2D scaleVelocity, velocity, acceleration;
	Object2D() : scaleVelocity(0, 0), velocity(0, 0), acceleration(0, 0) {}
	SimulationResult Simulation(float dt);
	void FixSimulation(SimulationResult result) {
		_simulationResult = std::make_unique<SimulationResult>(result);
	}
	void MoveAndSlide(float dt, const MoveAndSlideOption& option = {});
	void SetRotationDegree(double degree) {
		RotateDegree(degree);
	}
	void SetRotationRadian(double radian) {
		SetRotation(radian);
	}
	void SetRotationTurn(double turn) {
		RotateTurn(turn);
	}
	void SetAcceleration(const Vector2D& a) {
		acceleration = a;
	}
	void SetVelocity(const Vector2D& v) {
		velocity = v;
	}
	void SetScaleVelocity(const Vector2D& sv) {
		scaleVelocity = sv;
	}
};
