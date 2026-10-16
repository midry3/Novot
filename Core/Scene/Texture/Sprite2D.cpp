#include "Sprite2D.h"
#include <PropertyManager.h>
#include <NovotCore.h>
#include <Graphic/2D.h>

void Sprite2D::_Release() {
	if (_texture) {
		_texture->Release();
	}
}

void Sprite2D::_Draw() const {
	auto pos = GetDrawPosition();
	auto s = GetScale();
	int cx_in_image = static_cast<int>(_size.x / 2.0);
	int cy_in_image = static_cast<int>(_size.y / 2.0);

	const auto& pShader = GetPixelShader();

	if (pShader) {
		float uMin, vMin, uMax, vMax;
		GetUVRect(uMin, vMin, uMax, vMax);
		NovotRenderer::DrawRotaGraph3Shader(
			static_cast<int>(pos.x + cx_in_image * std::abs(s.x)),
			static_cast<int>(pos.y + cy_in_image * std::abs(s.y)),
			cx_in_image,
			cy_in_image,
			s.x,
			s.y,
			rotation,
			_handle,
			TRUE,
			pShader->GetHandle(),
			uMin, vMin, uMax, vMax
		);
	}
	else {
		DrawRotaGraph3(
			static_cast<int>(pos.x + cx_in_image * std::abs(s.x)),
			static_cast<int>(pos.y + cy_in_image * std::abs(s.y)),
			cx_in_image,
			cy_in_image,
			s.x,
			s.y,
			rotation,
			_handle,
			TRUE
		);
	}
}

void Sprite2D::ChangeStretch(StretchType type) {
	stretch = type;
	if (_size.x != 0 && _size.y != 0) {
		AdjustScale();
	}
}
