#include "Asset.h"
#include "NovotCore.h"
#include <DxLib.h>

std::vector<uint8_t> Asset::GetData() {
	std::vector<uint8_t> data;
	FILE* fp = nullptr;
	fs::path filePath = "Assets" / file;
	fopen_s(&fp, filePath.string().c_str(), "rb");
	if (fp == nullptr) {
		return data;
	}
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	data.resize(size);
	fread(data.data(), 1, size, fp);
	fclose(fp);
	return data;
}

int Asset::Load() {
	DebuggerWriteLine("Loading asset: " + file.string());
	if (!IsLoaded()) {
		_handle = CreateGraphFromMem(
			GetData().data(),
			static_cast<int>(GetData().size()),
			&_handle
		);
		if (_handle == -1) {
			DebuggerWriteLine("Failed to load asset: " + file.string());
			_handle = -2;
		}
		else {
			DebuggerWriteLine("Asset loaded successfully: " + file.string());
		}
	}
	return _handle;
}

void Asset::Release() {
	if (_handle != -1) {
		DeleteGraph(_handle);
		_handle = -1;
		DebuggerWriteLine("Released asset: " + file.string());
	}
}