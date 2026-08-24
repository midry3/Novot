#pragma once
#include <MathUtils.h>
#include <Singleton.h>
#include "MouseEvent.h"


class _Mouse : public Singleton<_Mouse> {
protected:
	MouseEvent _currentMouseEvent;
public:
	void UpdateMouseEvent();
	MouseEvent GetMouseEvent() const {
		return _currentMouseEvent;
	}
};

#define Mouse _Mouse::GetInstance()