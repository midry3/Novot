#include "TextLabel.h"

namespace {
	size_t GetCharacterLength(const std::string& value, size_t index) {
		if (index >= value.size()) return 0;
		const unsigned char character = static_cast<unsigned char>(value[index]);

		if (character < 0x80) {
			return 1;
		}

		// UTF-8 lead byte check
		if (character >= 0xC2 && character <= 0xDF) {
			return index + 1 < value.size() ? 2 : 1;
		}
		if (character >= 0xE0 && character <= 0xEF) {
			return index + 2 < value.size() ? 3 : (value.size() - index);
		}
		if (character >= 0xF0 && character <= 0xF4) {
			return index + 3 < value.size() ? 4 : (value.size() - index);
		}

		// Shift_JIS lead byte check (fallback)
		if ((character >= 0x81 && character <= 0x9F) ||
			(character >= 0xE0 && character <= 0xFC)) {
			return index + 1 < value.size() ? 2 : 1;
		}
		return 1;
	}

	std::vector<std::string> WrapText(const std::string& value, int width, int fontHandle) {
		std::vector<std::string> lines;
		std::string line;

		if (width <= 0) {
			lines.push_back(value);
			return lines;
		}

		for (size_t index = 0; index < value.size();) {
			if (value[index] == '\r' || value[index] == '\n') {
				lines.push_back(line);
				line.clear();
				if (value[index] == '\r' && index + 1 < value.size() && value[index + 1] == '\n') {
					++index;
				}
				++index;
				continue;
			}

			const size_t characterLength = GetCharacterLength(value, index);
			const std::string character = value.substr(index, characterLength);
			const std::string candidate = line + character;
			const int candidateWidth = GetDrawNStringWidthToHandle(
				candidate.c_str(),
				static_cast<int>(candidate.size()),
				fontHandle,
				FALSE
			);
			if (width > 0 && !line.empty() && candidateWidth > width) {
				lines.push_back(line);
				line.clear();
			}

			line += character;
			index += characterLength;
		}

		lines.push_back(line);
		return lines;
	}
}

std::string TextLabel::DefaultFont = "";

void TextLabel::_Load() {
	SetFont(DefaultFont);
}

void  TextLabel::_Draw() const {
	if (_fontHandle == -1) {
		return;
	}

	int lineSpacing = GetFontLineSpaceToHandle(_fontHandle);
	if (lineSpacing <= 0) {
		lineSpacing = _fontSize;
	}

	const auto s = GetDrawScale();
	const auto lines = WrapText(text, _width, _fontHandle);
	for (size_t index = 0; index < lines.size(); ++index) {
		const auto linePosition = GetDrawPoint(Vector2D(0, static_cast<double>(index * lineSpacing)));
		DrawRotaStringToHandle(
			static_cast<int>(linePosition.x),
			static_cast<int>(linePosition.y),
			s.x, s.y,
			0, 0,
			GetRotation(),
			foregroundColor,
			_fontHandle,
			foregroundColor,
			FALSE,
			lines[index].c_str()
		);
	}
}

void TextLabel::_Release() {
	if (_fontHandle != -1) {
		DeleteFontToHandle(_fontHandle);
		_fontHandle = -1;
	}
}
