#include "TextureButton.h"

void TextureButton::_Load() {
	Button::_Load();

	_sprite = CreateScene<Sprite2D>();
}

TextureButton* TextureButton::SetTexture(const SharedPtr<Texture>& texture) {
	_sprite->SetTexture(texture);

	const auto& textureSize = _sprite->GetSize();

	auto& mcol = GetMouseCollision();
	mcol.ClearVertices();
	mcol.AddVertices({
		{ 0, 0 },
		{ textureSize.x, 0 },
		{ textureSize.x, textureSize.y },
		{ 0, textureSize.y }
		});
	return this;
}

SharedPtr<Sprite2D> TextureButton::GetSprite() const {
	return _sprite;
}