#pragma once
#include <Scene.h>

class TextLabel : public Scene {
protected:
	int _fontHandle = -1;
	int _fontSize = 24;
	int _fontThickness = 3;
	std::string _fontName = "";
	int _width = -1;
	void _Load() override;
	void _Draw() const override;
	void _Release() override;
public:
	static std::string DefaultFont;
	std::string text;
	unsigned int foregroundColor = GetColor(255, 255, 255);

	TextLabel* SetText(const std::string& newText) {
		text = newText;
		return this;
	}

	TextLabel* AddText(const std::string& additionalText) {
		text += additionalText;
		return this;
	}

	TextLabel* SetForegroundColor(unsigned int color) {
		foregroundColor = color;
		return this;
	}

	void ApplyFont() {
		if (_fontHandle != -1) {
			DeleteFontToHandle(_fontHandle);
		}
		_fontHandle = CreateFontToHandle(_fontName.c_str(), _fontSize, _fontThickness);
	}

	TextLabel* SetFont(const std::string fontName) {
		_fontName = fontName;
		ApplyFont();
		return this;
	}

	TextLabel* SetFontSize(int size) {
		_fontSize = size;
		ApplyFont();
		return this;
	}

	TextLabel* SetFontThickness(int thickness) {
		_fontThickness = thickness;
		ApplyFont();
		return this;
	}

	TextLabel* SetWidth(int width) {
		_width = width;
		return this;
	}

	void Clear() {
		text.clear();
	}
};