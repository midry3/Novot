#include "ProgressSprite2D.h"
#include <Graphic/2D.h>
#include <Math/Utils.h>
#include <Animation/Tween.h>

void ProgressSprite2D::_Draw() const {
	auto pos = GetDrawPosition();
	auto s = GetScale();
	int cx_in_image = static_cast<int>(size.x / 2.0);
	int cy_in_image = static_cast<int>(size.y / 2.0);
	float uMin, vMin, uMax, vMax;
	GetUVRect(uMin, vMin, uMax, vMax);

	float progressWidth = (uMax - uMin) * _progress;
	
	const auto& pShader = GetPixelShader();
	if (pShader) {
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
			uMin, vMin, uMin + progressWidth, vMax
		);
	}
	else {
		DrawRectRotaGraph3(
			static_cast<int>(pos.x + cx_in_image * std::abs(s.x)),
			static_cast<int>(pos.y + cy_in_image * std::abs(s.y)),
			0, 0,
			size.x * _progress, size.y,
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

ProgressSprite2D* ProgressSprite2D::SetProgress(double progress) {
	_actualProgress = Clamp(progress, 0.0, 1.0);
	_progress = _actualProgress;
	return this;
}

ProgressSprite2D* ProgressSprite2D::SetProgressAnimated(double progress) {
	_actualProgress = Clamp(progress, 0.0, 1.0);
	const auto& tg = CreateTweenGroup();
	tg->CreateTween(&_progress, _actualProgress, _animationTime, Easing::EaseInQuad);
	return this;
}