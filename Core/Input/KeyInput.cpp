#include "KeyInput.h"
#include <algorithm>
#include <DxLib.h>

void _KeyInput::Update() {
	std::copy(std::begin(_currentKeyState), std::end(_currentKeyState), std::begin(_prevKeyState));
	GetHitKeyStateAll(_currentKeyState);
	_justPressedKey = GetInputChar(TRUE);
}

bool _KeyInput::IsKeyPressing(int key) {
	return 0 <= key && key < KEY_LENGTH && _currentKeyState[key];
}

bool _KeyInput::IsKeyPressingPrev(int key) {
	return 0 <= key && key < KEY_LENGTH && _prevKeyState[key];
}

bool _KeyInput::IsKeyJustPressed(int key) const {
	return 0 <= key && key < KEY_LENGTH && _currentKeyState[key] && not _prevKeyState[key];
}