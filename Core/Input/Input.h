#pragma once
#include <Singleton.h>
#include <Windows.h>

class _KeyInput : public Singleton<_KeyInput> {
protected:
	static constexpr int KEY_LENGTH = 256;
	TCHAR _justPressedKey;
	char _currentKeyState[KEY_LENGTH] = {}, _prevKeyState[KEY_LENGTH] = {};
public:
	void Update();
	bool IsKeyPressing(char key);
	bool IsKeyPressingPrev(char key);
	bool IsRawKeyPressing(int key) const { return key >= 0 && key < KEY_LENGTH && _currentKeyState[key] != 0; }
	bool IsRawKeyJustPressed(int key) const { return key >= 0 && key < KEY_LENGTH && _currentKeyState[key] && not _prevKeyState[key]; }
	[[nodiscard]] TCHAR GetJustInputKey() const {
		return _justPressedKey;
	}
};

#define KeyInput _KeyInput::GetInstance()