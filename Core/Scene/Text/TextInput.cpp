#include "TextInput.h"

void TextInput::_Load() {
	TextLabel::_Load();
	
	_mouseCollision = CreateScene<MouseCollision>();
	AdjustMouseCollisionArea();
}

void TextInput::_Update(float dt) {
	TextLabel::_Update(dt);
}

TextInput* TextInput::SetSize(const Vector2D& size) {
	_size = size;
	AdjustMouseCollisionArea();

	SetWidth(static_cast<int>(size.x));
	return this;
}

void TextInput::AdjustMouseCollisionArea() {
	if (_mouseCollision) {
		_mouseCollision->ClearVertices();
		_mouseCollision->AddVertices({ {0, 0}, {GetRawSize().x, 0}, {GetRawSize().x, GetRawSize().y}, {0, GetRawSize().y} });
	}
}