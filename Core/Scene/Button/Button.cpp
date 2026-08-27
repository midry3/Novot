#include "Button.h"
#include <Mouse/Mouse.h>

void Button::_Load() {
	collision = CreateScene<MouseCollision>();
	collision->LeftPressed.Connect([this](const MouseEvent& event) {
		LeftPressed.Emit(event);
		});
	collision->LeftReleased.Connect([this](const MouseEvent& event) {
		LeftReleased.Emit(event);
		});
	collision->MiddlePressed.Connect([this](const MouseEvent& event) {
		MiddlePressed.Emit(event);
		});
	collision->MiddleReleased.Connect([this](const MouseEvent& event) {
		MiddleReleased.Emit(event);
		});
	collision->RightPressed.Connect([this](const MouseEvent& event) {
		RightPressed.Emit(event);
		});
	collision->RightReleased.Connect([this](const MouseEvent& event) {
		RightReleased.Emit(event);
		});
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