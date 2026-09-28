#include "Asset.h"
#include "NovotCore.h"
#include "AssetManager.h"

std::vector<uint8_t> Asset::GetData() {
	return AssetManager.ReadData(file.string());
}

std::string Asset::GetDataAsString() {
	return AssetManager.ReadAsString(file.string());
}

fs::path Asset::GetPath() const {
	return AssetManager.GetPath(file);
}

fs::path Asset::GetPathRelatedToExe() const {
	wchar_t exePath[MAX_PATH];
	GetModuleFileNameW(nullptr, exePath, MAX_PATH);

	return fs::path(exePath).parent_path() / file;
}

int Asset::GetHandle() {
	return Load();
}

int Asset::ReLoad() {
	Release();
	return Load();
}