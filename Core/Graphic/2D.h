#pragma once
#include <MathUtils.h>

namespace NovotRenderer {
	void DrawLine(const Vector2D& p1, const Vector2D& p2, unsigned int color, int thickness);
	void DrawPolygon(const std::vector<Vector2D>& vertices, unsigned int color, bool filled = false);
	void DrawRect(const Vector2D& topLeft, const Vector2D& bottomRight, unsigned int color, bool filled = false);
};