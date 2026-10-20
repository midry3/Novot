#include "ColorRect.h"
#include <Graphic/2D.h>

void ColorRect::_Draw() const {
	auto pos = GetDrawPosition();
	auto s = GetDrawScale();
	NovotRenderer::DrawRect(pos, pos + size * s, color, true);
}