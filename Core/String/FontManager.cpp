#include "FontManager.h"
#include <gdiplus.h>
#include <iostream>
#include <string>

#pragma comment(lib, "gdiplus.lib")

namespace {
    std::string GetFontFamilyNameFromTTF(const fs::path& ttfFilePath) {
        using namespace Gdiplus;

        // GDI+ の初期化
        GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        std::string fontName = "";

        {
            PrivateFontCollection fontCollection;
            if (fontCollection.AddFontFile(ttfFilePath.c_str()) == Ok) {
                int numFamilies = 0;
                numFamilies = fontCollection.GetFamilyCount();
                if (numFamilies > 0) {
                    FontFamily family;
                    int numFound = 0;
                    fontCollection.GetFamilies(1, &family, &numFound);

                    WCHAR familyName[LF_FACESIZE];
                    family.GetFamilyName(familyName, LANG_NEUTRAL);

                    char nameBuf[256] = {};
                    WideCharToMultiByte(CP_UTF8, 0, familyName, -1, nameBuf, sizeof(nameBuf), NULL, NULL);
                    fontName = nameBuf;
                }
            }
        }

        GdiplusShutdown(gdiplusToken);

        return fontName;
    }
}

void _FontManager::AddFont(const fs::path& font) {
    const auto path = AssetManager.ResolvePath(font);
    AddFontResourceExA(path.string().c_str(), FR_PRIVATE, NULL);
	_fontNames.push_back(GetFontFamilyNameFromTTF(path));
}

std::string _FontManager::GetFont(const int index) const {
	return _fontNames.at(index);
}