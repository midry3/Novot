#include "Object2D.h"
#include <Collision/CollisionManager.h>

void Object2D::_Load() {
	if (not collision) {
		CreateCollision();
	}
}

void Object2D::_Update(float dt) {
	MoveAndSlide(dt);
}

std::shared_ptr<Collision> Object2D::CreateCollision() {
	collision = CollisionManager.New();
	collision->SetPassable(true);
	AddScene(collision);
	return collision;
}

SimulationResult Object2D::Simulation(float dt) {
	Vector2D _position = position, _velocity = velocity, _acceleration = acceleration, _scale = scale, _scaleVelocity = scaleVelocity;
	_velocity += _acceleration * dt;
	_position += _velocity * dt;
	_scale = _scale.Sign() * (_scaleVelocity * dt + _scale.Abs());
	auto colPos = collision->GetPosition();
	collision->SetPosition(_position - position);
	if (auto col = CollisionManager.GetCollidingWall(*collision)) {
		for (int i = 1; i <= _maxClimbHeight; ++i) {
			_position.y -= 1;
			collision->SetPosition(_position - position);
			if (not CollisionManager.IsCollidingWithWall(*collision)) {
				goto RETURN;
			}
		}
		_position = prePosition;
		_velocity = Vec2D_0;
	}
RETURN:
	collision->SetPosition(colPos);
	_simulationResult = std::make_unique<SimulationResult>();
	_simulationResult->position = _position;
	_simulationResult->velocity = _velocity;
	_simulationResult->acceleration = _acceleration;
	_simulationResult->scale = _scale;
	_simulationResult->scaleVelocity = _scaleVelocity;
	return *_simulationResult;
}

void Object2D::MoveAndSlide(float dt, const MoveAndSlideOption& option) {
	if (not _simulationResult) {
		Simulation(dt);
	}
	if (option.positionX) {
		position.x = _simulationResult->position.x;
	}
	if (option.positionY) {
		position.y = _simulationResult->position.y;
	}
	if (option.velocityX) {
		velocity.x = _simulationResult->velocity.x;
	}
	if (option.velocityY) {
		velocity.y = _simulationResult->velocity.y;
	}
	if (option.accelerationX) {
		acceleration.x = _simulationResult->acceleration.x;
	}
	if (option.accelerationY) {
		acceleration.y = _simulationResult->acceleration.y;
	}
	if (option.scaleX) {
		scale.x = _simulationResult->scale.x;
	}
	if (option.scaleY) {
		scale.y = _simulationResult->scale.y;
	}
	if (option.scaleVelocityX) {
		scaleVelocity.x = _simulationResult->scaleVelocity.x;
	}
	if (option.scaleVelocityY) {
		scaleVelocity.y = _simulationResult->scaleVelocity.y;
	}
	auto gpos = GetGlobalPosition();
	prePosition = position;
	preGlobalPosition = gpos;
	_simulationResult.reset();
}