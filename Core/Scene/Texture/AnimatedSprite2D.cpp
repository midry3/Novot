#include "AnimatedSprite2D.h"

void AnimatedSprite2D::_Load() {
	_spriteSheetManager.AnimationFinished.Connect([this](const int index) {
		AnimationFinished.Emit(index);
		});
	_spriteSheetManager.FrameChanged.Connect([this](int frame) {
		FrameChanged.Emit(frame);
		});
	FrameChanged.Connect([this](int frame) {
		if (auto sheet = _spriteSheetManager.GetCurrentSpriteSheet()) {
			SetTexture(sheet->GetCurrentFrame());
		}
		});
	if (_texture) {
		_handle = _texture->Load();
	}
}

void AnimatedSprite2D::_Release() {
	_spriteSheetManager.Release();
}

void AnimatedSprite2D::_Update(float dt) {
	if (auto sheet = _spriteSheetManager.GetCurrentSpriteSheet()) {
		sheet->UpdateFrame(dt);
	}
}

void AnimatedSprite2D::GetUVRect(float& uMin, float& vMin, float& uMax, float& vMax) const {
	if (auto sheet = _spriteSheetManager.GetCurrentSpriteSheet()) {
		sheet->GetUVRect(uMin, vMin, uMax, vMax);
	}
	else {
		Sprite2D::GetUVRect(uMin, vMin, uMax, vMax);
	}
}