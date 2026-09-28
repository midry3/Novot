#pragma once
#include <Scene.h>
#include <Math/Polygon.h>

class ColorRect : public Scene {
protected:
	PolygonInfo _polygon;
	void _Draw() const override;
public:
	Vector2D size;
	unsigned int color = GetColor(255, 255, 255);

	ColorRect* SetColor(unsigned int c) {
		color = c;
		return this;
	}

	ColorRect* SetSize(const Vector2D& s) {
		size = s;
		return this;
	}
};