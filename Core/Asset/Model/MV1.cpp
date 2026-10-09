#include "MV1.h"
#include <DxLib.h>

int MV1::Load() {
	if (_handle != -1) {
		return _handle;
	}
	_handle = MV1LoadModel(file.string().c_str());
	return _handle;
}

void MV1::Release() {
	if (IsLoaded()) {
		MV1DeleteModel(_handle);
		_handle = -1;
	}
}