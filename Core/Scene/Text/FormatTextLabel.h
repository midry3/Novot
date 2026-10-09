#pragma once
#include "TextLabel.h"

struct TextFormat {
	std::string text;
	unsigned int color;
};

class FormatTextLabel : public TextLabel {
protected:
	struct TextSegment {
		std::string text;
		Vector2D position;
		unsigned int color;
		bool underline = false;
	};
	std::map<std::string, TextFormat> _formats;
	std::vector<TextSegment> _segments;

	void _Draw() const override;
public:
	FormatTextLabel* SetText(const std::string& newText) override;
	FormatTextLabel* AddText(const std::string& additionalText) override;
	FormatTextLabel* SetForegroundColor(unsigned int color) {
		TextLabel::SetForegroundColor(color);
		ParseText();
		return this;
	}
	FormatTextLabel* SetFont(const std::string fontName) {
		TextLabel::SetFont(fontName);
		ParseText();
		return this;
	}
	FormatTextLabel* SetFontSize(int size) {
		TextLabel::SetFontSize(size);
		ParseText();
		return this;
	}
	FormatTextLabel* SetFontThickness(int thickness) {
		TextLabel::SetFontThickness(thickness);
		ParseText();
		return this;
	}
	FormatTextLabel* SetWidth(int width) {
		TextLabel::SetWidth(width);
		ParseText();
		return this;
	}
	FormatTextLabel* SetFormat(const std::string& key, const TextFormat& format);
	void ParseText();
};