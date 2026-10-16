#include "KeyMapping.h"

void _KeyMapping::Update() {
	for (int i = 0; i < 256; i++) {
		prevKeyState[i] = keyState[i];
	}
	GetHitKeyStateAll(keyState);
}

bool _KeyMapping::IsActionPressing(const int action) {
	if (not mapping.contains(action)) return false;
	for (int key : mapping[action]) {
		if (keyState[key]) {
			_isDetected = true;
			return true;
		}
	}
	return false;
}

bool _KeyMapping::IsActionJustPressed(const int action) {
	if (not mapping.contains(action)) return false;
	for (int key : mapping[action]) {
		if (keyState[key] && not prevKeyState[key]) {
			_isDetected = true;
			return true;
		}
	}
	return false;
}