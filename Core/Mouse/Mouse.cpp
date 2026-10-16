#include "Mouse.h"
#include <NovotCore.h>
#include <DxLib.h>
#include <Camera.h>

void  _Mouse::UpdateMouseEvent() {
	_handled = false;
	int x, y;
	auto input = GetMouseInput();
	auto preEvent = _currentMouseEvent;
	GetMousePoint(&x, &y);
	auto offset = GetMouseWheelRotVol();
	if (KeyMapping.IsActionPressing(DefautKeyMapping::KEY_SHIFT)) {
		_currentMouseEvent.scrollOffset = Vector2D(static_cast<double>(offset), 0);
	}
	else {
		_currentMouseEvent.scrollOffset = Vector2D(0, static_cast<double>(offset));
	}
	if (offset != 0) {
		Scrolled.Emit(_currentMouseEvent);
	}
	_currentMouseEvent.position = Vector2D(static_cast<double>(x), static_cast<double>(y));
	if (const auto scene = SceneManager.GetCurrentScene()) {
		_currentMouseEvent.globalPosition = _currentMouseEvent.position + scene->GetGlobalPosition();
	}
	else {
		_currentMouseEvent.globalPosition = _currentMouseEvent.position;
	}
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
	if (preEvent.position != _currentMouseEvent.position) {
		Moved.Emit(_currentMouseEvent);
	}
}