#include "ColorPolygon.h"
#include <Graphic/2D.h>

void ColorPolygon::_Draw() const {
	std::vector<Vector2D> verts;
	for (const auto& v : _polygon.GetVertices()) {
		verts.push_back(GetDrawPoint(v));
	}
	NovotRenderer::DrawPolygon(verts, color, true);
}
