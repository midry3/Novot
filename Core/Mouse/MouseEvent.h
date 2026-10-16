#pragma once
#include <Math/Vector2D.h>

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
	Vector2D position, globalPosition, delta, scrollOffset;
	MouseButton button;
	MouseButtonState state;
	bool just;
};