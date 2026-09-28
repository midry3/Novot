#pragma once
#include <Singleton.h>
#include <Asset/AssetManager.h>
#include <Windows.h>

class _FontManager : public Singleton<_FontManager> {
public:
	void AddFont(const std::string& font) {
		AddFontResourceExA(AssetManager.ResolvePath(font).string().c_str(), FR_PRIVATE, NULL);
	}
};

#define FontManager _FontManager::GetInstance()