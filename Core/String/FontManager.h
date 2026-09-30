#pragma once
#include <Singleton.h>
#include <Asset/AssetManager.h>
#include <Windows.h>

namespace {
	std::string GetFontFamilyNameFromTTF(const fs::path& ttfFilePath);
}

class _FontManager : public Singleton<_FontManager> {
private:
	std::vector<std::string> _fontNames;
public:
	void AddFont(const fs::path& font);
	std::string GetFont(const int index) const;
};

#define FontManager _FontManager::GetInstance()