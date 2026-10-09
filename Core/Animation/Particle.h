#pragma once
#include <Math/Vector2D.h>

struct ParticleParam {
	Vector2D position = Vector0, velocity = Vector0;
	Vector2DFunction accel;
	std::function<double(double)> rotationAccel;
	double rotation = 0, rotationVelocity = 0;

	void Update(double dt) {
		accel.Update(dt);
		velocity += accel * dt;
		position += velocity * dt;

		rotationVelocity += rotationAccel(dt) * dt;
		rotation += rotationVelocity * dt;
	}
};

class Particle2D {
protected:
	ParticleParam _param;

public:
	Particle2D() = default;
	Particle2D(ParticleParam param) : _param(param){}
	~Particle2D() = default;

	void Update(double dt);
	Vector2D GetPosition();
	Vector2D* GetPositionPtr();
	Vector2D GetVelocityPtr();
	double GetRotation();
};