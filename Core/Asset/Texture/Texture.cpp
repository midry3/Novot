#include "Texture.h"
#include <DxLib.h>
#include <Debugger/Debugger.h>

int Texture::Load() {
	if (not IsLoaded()) {
		_handle = CreateGraphFromMem(
			GetData().data(),
			static_cast<int>(GetData().size())
		);
		if (_handle == -1) {
			DebuggerWriteLine("Failed to load asset: " + file.string());
			_handle = -2;
		}
	}
	return _handle;
}

void Texture::Release() {
	if (IsLoaded()) {
		DeleteGraph(_handle);
		_handle = -1;
	}
}

const Vector2D Texture::GetSize() const {
	if (IsLoaded()) {
		int width = 0, height = 0;
		GetGraphSize(_handle, &width, &height);
		return Vector2D(width, height);
	}
	return Vector2D(0, 0);
}