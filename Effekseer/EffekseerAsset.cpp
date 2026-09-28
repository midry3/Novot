#include "EffekseerAsset.h"
#include <EffekseerForDXLib.h>
#include <NovotCore.h>

int EffekseerAsset::Load() {
	if (not IsLoaded()) {
		_handle = LoadEffekseerEffect(GetPath().c_str());
		if (_handle == -1) {
			DebuggerWriteLine("Failed to load asset: " + file.string());
			_handle = -2;
		}
	}
	return _handle;
}

void EffekseerAsset::Release() {
	if (IsLoaded()) {
		DeleteEffekseerEffect(_handle);
		_handle = -1;
	}
}