#include "KeyMapping.h"

void _KeyMapping::Update() {
	for (int i = 0; i < 256; i++) {
		prevKeyState[i] = keyState[i];
	}
	GetHitKeyStateAll(keyState);
}

bool _KeyMapping::IsActionPressing(const std::string& action) {
	if (mapping.find(action) == mapping.end()) return false;
	for (int key : mapping[action]) {
		if (keyState[key]) {
			isDetected = true;
			return true;
		}
	}
	return false;
}

bool _KeyMapping::IsActionJustPressed(const std::string& action) {
	if (mapping.find(action) == mapping.end()) return false;
	for (int key : mapping[action]) {
		if (keyState[key] && not prevKeyState[key]) {
			isDetected = true;
			return true;
		}
	}
	return false;
}