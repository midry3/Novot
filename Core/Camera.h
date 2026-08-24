#pragma once
#include <Singleton.h>
#include <MathUtils.h>

class _Camera : public Singleton<_Camera> {
protected:
	Vector2D point = Vector0;
public:
	void SetPoint(const Vector2D& point) {
		this->point = point;
	}
	Vector2D GetPoint() const {
		return point;
	}
};

#define Camera _Camera::GetInstance()