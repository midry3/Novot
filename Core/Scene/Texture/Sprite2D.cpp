#include "Sprite2D.h"
#include <math.h>
#include <PropertyManager.h>
#include <NovotCore.h>

void Sprite2D::_Load() {
	
}

void Sprite2D::_Release() {
	if (texture) {
		texture->Release();
	}
}

void Sprite2D::_Draw() const {
	DebuggerSetProperty("scale", std::to_string(scale.x) + ", " + std::to_string(scale.y));
	auto pos = GetDrawPosition();
	DrawRotaGraph3(
		static_cast<int>(pos.x + size.x * scale.x / 2),
		static_cast<int>(pos.y + size.y * scale.y / 2),
		0,
		0,
		scale.x,
		scale.y,
		rotation,
		_handle,
		TRUE
	);
}

void Sprite2D::_Update(float dt) {

}

void Sprite2D::ChangeStretch(StretchType type) {
	stretch = type;
	if (size.x != 0 && size.y != 0) {
		AdjustScale();
	}
}

void Sprite2D::AdjustScale() {
	Vector2D wsize;
	float r;
	switch (stretch) {
	case StretchType::FIT:
		wsize = PropertyManager.GetRealWindowSize();
		r = min(wsize.x / size.x, wsize.y / size.y)/2;
		scale = Vector2D(r, r);
		DebuggerWriteLine("Changed stretch to FIT. Scale: " + std::to_string(scale.x * size.x) + ", " + std::to_string(scale.y * size.y));
		break;
	case StretchType::NONE:
		break;
	}
}