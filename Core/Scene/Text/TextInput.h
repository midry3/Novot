#pragma once
#include "TextLabel.h"
#include <Collision/MouseCollision.h>

class TextInput : public TextLabel {
protected:
	Vector2D _size;

	SharedPtr<MouseCollision> _mouseCollision;

	void _Load() override;
	void _Update(float dt) override;

	void AdjustMouseCollisionArea();
public:
	TextInput* SetSize(const Vector2D& size);

	[[nodiscard]] const Vector2D& GetRawSize() const override {
		return _size;
	}
};