#pragma once
#include "Button.h"
#include <Scene/Texture/Sprite2D.h>

class TextureButton : public Button {
private:
	SharedPtr<Sprite2D> _sprite;
protected:
	void _Load() override;
public:
	TextureButton* SetTexture(const SharedPtr<Texture>& texture);
	[[nodiscard]] SharedPtr<Sprite2D> GetSprite() const;
};