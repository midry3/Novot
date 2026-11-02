#include "MV1.h"
#include <Backend/Backend.h>

int MV1::Load() {
	if (_handle != -1) {
		return _handle;
	}
	_handle = CoreBackend::Model::Load(file.string().c_str());
	return _handle;
}

void MV1::Release() {
	if (IsLoaded()) {
		CoreBackend::Model::Release(_handle);
		_handle = -1;
	}
}