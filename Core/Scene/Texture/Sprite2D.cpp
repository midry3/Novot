#include "Sprite2D.h"
#include <PropertyManager.h>
#include <NovotCore.h>
#include <Graphic/2D.h>
#include <Backend/Backend.h>

void Sprite2D::_Release() {
	if (_texture) {
		_texture->Release();
	}
}

void Sprite2D::_Draw() const {
	auto pos = GetDrawPosition();
	auto s = GetDrawScale();
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
			GetRotation(),
			_handle,
			1,
			pShader->GetHandle(),
			uMin, vMin, uMax, vMax,
			GetAlpha() / 255.0f
		);
	}
	else {
		CoreBackend::Drawing::DrawSprite(
			static_cast<int>(pos.x + cx_in_image * std::abs(s.x)),
			static_cast<int>(pos.y + cy_in_image * std::abs(s.y)),
			cx_in_image,
			cy_in_image,
			s.x,
			s.y,
			GetRotation(),
			_handle,
			1
		);
	}
}

void Sprite2D::ChangeStretch(StretchType type) {
	stretch = type;
	if (_size.x != 0 && _size.y != 0) {
		AdjustScale();
	}
}

void Sprite2D::AdjustScale() {
	Vector2D wsize;
	float r;
	switch (stretch) {
	case StretchType::FIT:
		wsize = PropertyManager.GetRealWindowSize();
		r = Min(wsize.x / GetRawSize().x, wsize.y / GetRawSize().y);
		SetScale(r);
		break;
	case StretchType::NONE:
		break;
	}
}

void  Sprite2D::AdjustSize() {
	int x, y;
	CoreBackend::Texture::Size(GetHandle(), &x, &y);
	_size = Vec2(x, y);
	_handle = GetHandle();
	AdjustScale();
}