#include "ColorPolygon.h"
#include <Graphic/2D.h>

void ColorPolygon::_Draw() const {
	auto pos = GetDrawPosition();
	auto s = GetDrawScale();
	std::vector<Vector2D> verts;
	for (const auto& v : _polygon.GetVertices()) {
		verts.push_back(pos + v * s);
	}
	NovotRenderer::DrawPolygon(verts, color, true);
}