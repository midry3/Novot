#pragma once
#include <Event/Signal.h>
#include <Singleton.h>
#include "MouseEvent.h"


class _Mouse : public Singleton<_Mouse> {
protected:
	MouseEvent _currentMouseEvent;
public:
	Signal<const MouseEvent&> LeftPressed, LeftReleased, MiddlePressed, MiddleReleased, RightPressed, RightReleased;
	void UpdateMouseEvent();
	MouseEvent GetMouseEvent() const {
		return _currentMouseEvent;
	}
};

#define Mouse _Mouse::GetInstance()