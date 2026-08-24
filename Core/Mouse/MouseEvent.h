#pragma once
#include <MathUtils.h>

enum class MouseButton {
	Left,
	Middle,
	Right,
	None
};

enum class MouseButtonState {
	Pressed,
	Released,
	Moved
};

struct MouseEvent {
public:
	Vector2D position;
	MouseButton button;
	MouseButtonState state;
	bool just;
	Vector2D delta;
};