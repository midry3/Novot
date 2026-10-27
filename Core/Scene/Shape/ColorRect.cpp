#include "ColorRect.h"
#include <Graphic/2D.h>

void ColorRect::_Draw() const {
	const std::vector<Vector2D> vertices = {
		GetDrawPoint(Vec2D_0),
		GetDrawPoint(Vector2D(size.x, 0)),
		GetDrawPoint(size),
		GetDrawPoint(Vector2D(0, size.y))
	};
	NovotRenderer::DrawPolygon(vertices, color, true);
}
