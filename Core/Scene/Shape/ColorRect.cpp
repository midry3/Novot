#include "ColorRect.h"
#include <Graphic/2D.h>

void ColorRect::_Draw() const {
	auto pos = GetDrawPosition();
	NovotRenderer::DrawRect(pos, pos + size, color, true);
}