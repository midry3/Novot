#include "KeyMapping.h"
#include "Input.h"

bool _KeyMapping::IsActionPressing(const int action) {
	if (!_gameInputEnabled) return false;
	if (not mapping.contains(action)) return false;
	for (int key : mapping[action]) {
		if (KeyInput.IsKeyPressing(key)) {
			_isDetected = true;
			return true;
		}
	}
	return false;
}

bool _KeyMapping::IsActionJustPressed(const int action) {
	if (!_gameInputEnabled) return false;
	if (not mapping.contains(action)) return false;
	for (int key : mapping[action]) {
		if (KeyInput.IsKeyPressing(key) && not KeyInput.IsKeyPressingPrev(key)) {
			_isDetected = true;
			return true;
		}
	}
	return false;
}
