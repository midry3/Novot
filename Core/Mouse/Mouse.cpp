#include "Mouse.h"
#include <NovotCore.h>
#include <Camera.h>
#include <Backend/Backend.h>

void  _Mouse::UpdateMouseEvent() {
	_handled = false;
	int x, y;
	auto input = CoreBackend::MouseInput::Buttons();
	auto preEvent = _currentMouseEvent;
	CoreBackend::MouseInput::Position(&x, &y);
	auto offset = CoreBackend::MouseInput::Wheel();
	if (KeyMapping.IsActionPressing(DefautKeyMapping::KEY_SHIFT)) {
		_currentMouseEvent.scrollOffset = Vector2D(static_cast<double>(offset), 0);
	}
	else {
		_currentMouseEvent.scrollOffset = Vector2D(0, static_cast<double>(offset));
	}
	if (_eventsEnabled && offset != 0) {
		Scrolled.Emit(_currentMouseEvent);
	}
	_currentMouseEvent.position = Vector2D(static_cast<double>(x), static_cast<double>(y));
	_currentMouseEvent.globalPosition = Camera.ScreenToWorld(_currentMouseEvent.position);
	_currentMouseEvent.delta = _currentMouseEvent.position - preEvent.position;
	if (CoreBackend::MouseInput::IsLeft(input)) {
		_currentMouseEvent.button = MouseButton::Left;
		_currentMouseEvent.state = MouseButtonState::Pressed;
		if (_eventsEnabled) LeftPressed.Emit(_currentMouseEvent);
	}
	else if (CoreBackend::MouseInput::IsRight(input)) {
		_currentMouseEvent.button = MouseButton::Right;
		_currentMouseEvent.state = MouseButtonState::Pressed;
		if (_eventsEnabled) RightPressed.Emit(_currentMouseEvent);
	}
	else if (CoreBackend::MouseInput::IsMiddle(input)) {
		_currentMouseEvent.button = MouseButton::Middle;
		_currentMouseEvent.state = MouseButtonState::Pressed;
		if (_eventsEnabled) MiddlePressed.Emit(_currentMouseEvent);
	}
	else {
		if (preEvent.state == MouseButtonState::Pressed) {
			_currentMouseEvent.state = MouseButtonState::Released;
			switch (preEvent.button) {
			case MouseButton::Left:
				if (_eventsEnabled) LeftReleased.Emit(_currentMouseEvent);
				break;
			case MouseButton::Middle:
				if (_eventsEnabled) MiddleReleased.Emit(_currentMouseEvent);
				break;
			case MouseButton::Right:
				if (_eventsEnabled) RightReleased.Emit(_currentMouseEvent);
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
		if (_eventsEnabled) Moved.Emit(_currentMouseEvent);
	}
}
