#pragma once
#include <Event/Signal.h>
#include <Singleton.h>
#include "MouseEvent.h"


class _Mouse : public Singleton<_Mouse> {
protected:
	MouseEvent _currentMouseEvent;
	bool _handled = false;
public:
	Signal<const MouseEvent&> LeftPressed, LeftReleased, MiddlePressed, MiddleReleased, RightPressed, RightReleased, Moved, Scrolled;
	void UpdateMouseEvent();
	MouseEvent GetMouseEvent() const {
		return _currentMouseEvent;
	}
	bool IsAlreadyHandled() const {
		return _handled;
	}
	void Handled() {
		_handled = true;
	}
};

#define Mouse _Mouse::GetInstance()