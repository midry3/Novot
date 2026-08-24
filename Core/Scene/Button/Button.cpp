#include "Button.h"
#include <Mouse/Mouse.h>

void Button::_Load() {
	collision = CreateScene<MouseCollision>();
}

void Button::_Update(float dt) {
	auto mouseEvent = Mouse.GetMouseEvent();
	if (collision->IsWithin(mouseEvent.position)) {
		DebuggerSetProperty("Mouse Over Button", "True");
		collision->CallCallback(mouseEvent);
	}
	else {
		DebuggerSetProperty("Mouse Over Button", "False");
	}
}

void Button::_Draw() const {
	//auto pos = GetDrawPosition();
	//DrawString(pos.x, pos.y, "Button", GetColor(255, 255, 255));
}