#include "Object2D.h"
#include <Collision/CollisionManager.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

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

static bool GetMinimumTranslation(const Collision& moving, const Collision& obstacle, Vector2D& translation) {
	const auto movingVertices = moving.GetVertices();
	const auto obstacleVertices = obstacle.GetVertices();
	if (movingVertices.size() < 3 || obstacleVertices.size() < 3) return false;

	double minimumDepth = std::numeric_limits<double>::infinity();
	Vector2D bestTranslation{};
	auto testAxes = [&](const std::vector<Vector2D>& vertices) {
		for (size_t i = 0; i < vertices.size(); ++i) {
			const Vector2D edge = vertices[(i + 1) % vertices.size()] - vertices[i];
			const Vector2D axis = edge.Perpendicular().Normalized();
			if (axis.Length() == 0.0) continue;

			double movingMin = std::numeric_limits<double>::infinity();
			double movingMax = -std::numeric_limits<double>::infinity();
			double obstacleMin = std::numeric_limits<double>::infinity();
			double obstacleMax = -std::numeric_limits<double>::infinity();
			for (const auto& vertex : movingVertices) {
				const double projection = axis.Dot(vertex);
				movingMin = Min(movingMin, projection);
				movingMax = Max(movingMax, projection);
			}
			for (const auto& vertex : obstacleVertices) {
				const double projection = axis.Dot(vertex);
				obstacleMin = Min(obstacleMin, projection);
				obstacleMax = Max(obstacleMax, projection);
			}
			if (movingMax < obstacleMin || obstacleMax < movingMin) return false;

			const double moveNegative = movingMax - obstacleMin;
			const double movePositive = obstacleMax - movingMin;
			const double depth = Min(moveNegative, movePositive);
			if (depth < minimumDepth) {
				minimumDepth = depth;
				bestTranslation = axis * (moveNegative <= movePositive ? -depth : depth);
			}
		}
		return true;
	};

	if (!testAxes(movingVertices) || !testAxes(obstacleVertices) || !std::isfinite(minimumDepth)) return false;
	if (minimumDepth < 0.001) bestTranslation = bestTranslation.Normalized(0.001);
	translation = bestTranslation;
	return true;
}

SimulationResult& Object2D::Simulation(float dt) {
	Vector2D nextPosition = position;
	Vector2D nextVelocity = velocity;
	const Vector2D nextAcceleration = acceleration;
	Vector2D nextScale = scale;
	const Vector2D nextScaleVelocity = scaleVelocity;
	nextVelocity += nextAcceleration * dt;
	const Vector2D movement = nextVelocity * dt;
	nextScale = nextScale.Sign() * (nextScaleVelocity * dt + nextScale.Abs());

	const Vector2D collisionPosition = collision->GetPosition();
	const Vector2D parentScale = GetParentScale();
	const Vector2D objectScale = GetScale();
	auto placeAt = [&](const Vector2D& objectPosition) {
		const Vector2D objectDelta = objectPosition - position;
		const Vector2D worldDelta = objectDelta * parentScale;
		const Vector2D collisionDelta(objectScale.x == 0.0 ? 0.0 : worldDelta.x / objectScale.x,
			objectScale.y == 0.0 ? 0.0 : worldDelta.y / objectScale.y);
		collision->SetPosition(collisionPosition + collisionDelta);
	};
	auto collidesAt = [&](const Vector2D& candidate) {
		placeAt(candidate);
		const auto walls = CollisionManager.GetAllCollidingWall(*collision);
		for (const auto& wall : walls) {
			collision->RegisterPhysicalContact(wall);
			wall->RegisterPhysicalContact(collision);
		}
		return not walls.empty();
	};

	for (int attempt = 0; attempt < 8; ++attempt) {
		placeAt(nextPosition);
		const auto walls = CollisionManager.GetAllCollidingWall(*collision);
		for (const auto& wall : walls) {
			collision->RegisterPhysicalContact(wall);
			wall->RegisterPhysicalContact(collision);
		}
		if (walls.empty()) break;
		Vector2D shortestTranslation{};
		double shortestDistance = std::numeric_limits<double>::infinity();
		for (const auto& wall : walls) {
			Vector2D translation{};
			if (GetMinimumTranslation(*collision, *wall, translation) && translation.Length() < shortestDistance) {
				shortestTranslation = translation;
				shortestDistance = translation.Length();
			}
		}
		if (!std::isfinite(shortestDistance) || shortestDistance == 0.0) break;
		const Vector2D localTranslation(parentScale.x == 0.0 ? 0.0 : shortestTranslation.x / parentScale.x,
			parentScale.y == 0.0 ? 0.0 : shortestTranslation.y / parentScale.y);
		nextPosition += localTranslation;
	}

	// Resolve horizontal movement first and permit only the configured step-up height.
	if (movement.x != 0.0) {
		Vector2D candidate = nextPosition;
		candidate.x += movement.x;
		if (collidesAt(candidate)) {
			bool climbed = false;
			for (int step = 1; step <= _maxClimbHeight; ++step) {
				candidate.y = nextPosition.y - step;
				if (!collidesAt(candidate)) {
					nextPosition = candidate;
					climbed = true;
					break;
				}
			}
			if (!climbed) nextVelocity.x = 0.0;
		}
		else {
			nextPosition = candidate;
		}
	}

	// Resolve vertical movement independently so one blocked axis does not cancel the other.
	if (movement.y != 0.0) {
		Vector2D candidate = nextPosition;
		candidate.y += movement.y;
		if (collidesAt(candidate)) nextVelocity.y = 0.0;
		else nextPosition = candidate;
	}

	collision->SetPosition(collisionPosition);
	_simulationResult = std::make_unique<SimulationResult>();
	_simulationResult->position = nextPosition;
	_simulationResult->velocity = nextVelocity;
	_simulationResult->acceleration = nextAcceleration;
	_simulationResult->scale = nextScale;
	_simulationResult->scaleVelocity = nextScaleVelocity;
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
