#include "Button.h"
#include <Mouse/Mouse.h>

void Button::_Load() {
	collision = CreateScene<MouseCollision>();
	collision->AddVertices(areaVertices);
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
