#include "ColorPolygon.h"
#include <Graphic/2D.h>

void ColorPolygon::_Draw() const {
	auto pos = GetDrawPosition();
	NovotRenderer::DrawPolygon(_polygon.RelatedTo(pos).GetVertices(), color, true);
}