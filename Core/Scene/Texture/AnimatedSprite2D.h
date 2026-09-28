#pragma once
#include "Sprite2D.h"
#include <Asset/Texture/SpriteSheetManager.h>

class AnimatedSprite2D : public Sprite2D {
protected:
	std::string _currentAnimation;
	SpriteSheetManager _spriteSheetManager;
	void _Load() override;
	void _Update(float dt) override;
public:
	Signal<int> FrameChanged;
	Signal<std::string> AnimationChanged, AnimationFinished;
	std::string  GetCurrentAnimation() const {
		return _currentAnimation;
	}
	AnimatedSprite2D* Play(std::string anim, bool loop = false) {
		_currentAnimation = anim;
		_spriteSheetManager.SetCurrentAnimation(_currentAnimation);
		if (loop) {
			_spriteSheetManager[_currentAnimation]->SetLoop(true);
		}
		SetScale(_spriteSheetManager[_currentAnimation]->GetScale());
		_spriteSheetManager.Play();
		AnimationChanged(_currentAnimation);
		return this;
	}
	AnimatedSprite2D* AddAnimation(std::string anim, std::shared_ptr<SpriteSheet> spriteSheet) {
		_spriteSheetManager.RegisterSpriteSheet(anim, spriteSheet);
		return this;
	}
	[[nodiscard]] Vector2D GetCurrentSheetSize() const {
		return _spriteSheetManager.GetCurrentSheetSize();
	}
	[[nodiscard]] Vector2D GetCurrentFrameSize() const {
		return _spriteSheetManager.GetCurrentFrameSize();
	}
	[[nodiscard]] void GetUVRect(float& uMin, float& vMin, float& uMax, float& vMax) const override;
};