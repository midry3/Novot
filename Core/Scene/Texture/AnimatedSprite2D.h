#pragma once
#include "Sprite2D.h"
#include <Asset/Texture/SpriteSheetManager.h>

class AnimatedSprite2D : public Sprite2D {
protected:
	int _currentAnimation = 0;
	SpriteSheetManager _spriteSheetManager;
	Vector<String> _animationNames;
	void _Load() override;
	void _Release() override;
	void _Update(float dt) override;
public:
	Signal<int> FrameChanged, AnimationChanged, AnimationFinished;
	int GetCurrentAnimation() const {
		return _currentAnimation;
	}
	AnimatedSprite2D* Play(const int index, bool loop = false) {
		auto spriteSheet = _spriteSheetManager.GetSpriteSheet(index);
		if (not spriteSheet) {
			return this;
		}
		_currentAnimation = index;
		_spriteSheetManager.SetCurrentAnimation(_currentAnimation);
		if (loop) {
			spriteSheet->SetLoop(true);
		}
		SetScale(spriteSheet->GetScale());
		_spriteSheetManager.Play();
		AnimationChanged(_currentAnimation);
		return this;
	}
	AnimatedSprite2D* Play(const String& anim, bool loop = false) {
		auto it = std::find(_animationNames.begin(), _animationNames.end(), anim);
		if (it != _animationNames.end()) {
			return Play(std::distance(_animationNames.begin(), it), loop);
		}
		return this;
	}
	AnimatedSprite2D* AddAnimation(const int index, const SharedPtr<SpriteSheet>& spriteSheet) {
		_spriteSheetManager.RegisterSpriteSheet(index, spriteSheet);
		_animationNames.push_back(std::to_string(index));
		return this;
	}
	AnimatedSprite2D* AddAnimation(const String& anim, const SharedPtr<SpriteSheet>& spriteSheet) {
		_spriteSheetManager.RegisterSpriteSheet(_spriteSheetManager.GetAnimationCount(), spriteSheet);
		_animationNames.push_back(anim);
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