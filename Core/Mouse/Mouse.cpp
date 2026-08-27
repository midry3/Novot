#include "Mouse.h"
#include <NovotCore.h>
#include <DxLib.h>

void  _Mouse::UpdateMouseEvent() {
	int x, y;
	auto input = GetMouseInput();
	auto preEvent = _currentMouseEvent;
	GetMousePoint(&x, &y);
	_currentMouseEvent.position = Vector2D(static_cast<double>(x), static_cast<double>(y));
	_currentMouseEvent.delta = _currentMouseEvent.position - preEvent.position;
	if (input & MOUSE_INPUT_LEFT) {
		_currentMouseEvent.button = MouseButton::Left;
		_currentMouseEvent.state = MouseButtonState::Pressed;
		LeftPressed.Emit(_currentMouseEvent);
	}
	else if (input & MOUSE_INPUT_RIGHT) {
		_currentMouseEvent.button = MouseButton::Right;
		_currentMouseEvent.state = MouseButtonState::Pressed;
		RightPressed.Emit(_currentMouseEvent);
	}
	else if (input & MOUSE_INPUT_MIDDLE) {
		_currentMouseEvent.button = MouseButton::Middle;
		_currentMouseEvent.state = MouseButtonState::Pressed;
		MiddlePressed.Emit(_currentMouseEvent);
	}
	else {
		if (preEvent.state == MouseButtonState::Pressed) {
			_currentMouseEvent.state = MouseButtonState::Released;
			switch (preEvent.button) {
			case MouseButton::Left:
				LeftReleased.Emit(_currentMouseEvent);
				break;
			case MouseButton::Middle:
				MiddleReleased.Emit(_currentMouseEvent);
				break;
			case MouseButton::Right:
				RightReleased.Emit(_currentMouseEvent);
				break;
			}
		}
		else {
			_currentMouseEvent.button = MouseButton::None;
			_currentMouseEvent.state = MouseButtonState::Moved;
		}
	}
	_currentMouseEvent.just = (preEvent.state != _currentMouseEvent.state);

	DebuggerSetProperty("Mouse Position", "(" + std::to_string(_currentMouseEvent.position.x) + ", " + std::to_string(_currentMouseEvent.position.y) + ")");
	DebuggerSetProperty("Mouse Button", std::to_string(static_cast<int>(_currentMouseEvent.button)));
	DebuggerSetProperty("Mouse State", std::to_string(static_cast<int>(_currentMouseEvent.state)));
}