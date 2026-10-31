#pragma once
#include <Singleton.h>
#include <Windows.h>

class _KeyInput : public Singleton<_KeyInput> {
protected:
	static constexpr int KEY_LENGTH = 256;
	TCHAR _justPressedKey = '\0';
	char _currentKeyState[KEY_LENGTH] = {}, _prevKeyState[KEY_LENGTH] = {};
public:
	void Update();
	bool IsKeyPressing(int key);
	bool IsKeyPressingPrev(int key);
	bool IsKeyJustPressed(int key) const;
	[[nodiscard]] TCHAR GetJustInputKey() const {
		return _justPressedKey;
	}
};

#define KeyInput _KeyInput::GetInstance()