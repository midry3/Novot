#include "Input.h"
#include <algorithm>
#include <DxLib.h>

void _KeyInput::Update() {
	std::copy(std::begin(_currentKeyState), std::end(_currentKeyState), std::begin(_prevKeyState));
	GetHitKeyStateAll(_currentKeyState);
	_justPressedKey = GetInputChar(TRUE);
}

bool _KeyInput::IsKeyPressing(char key) {
	return _currentKeyState[key];
}

bool _KeyInput::IsKeyPressingPrev(char key) {
	return _prevKeyState[key];
}