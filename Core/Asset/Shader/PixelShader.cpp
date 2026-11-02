#include "PixelShader.h"
#include <NovotCore.h>
#include <Backend/Backend.h>

int PixelShader::Load() {
	if (_handle != -1) {
		return _handle;
	}
	auto data = GetData();
	_handle = CoreBackend::Shader::LoadPixel(GetPathRelatedToExe().string().c_str());
	if (_handle == -1) {
		_handle = -2;
		DebuggerWriteLine("Failed to load pixel shader: " + GetPath().string());
	}
	return _handle;
}

void PixelShader::Release() {
	if (_handle != -1 && _handle != -2) {
		CoreBackend::Shader::Release(_handle);
	}
	_handle = -1;
}