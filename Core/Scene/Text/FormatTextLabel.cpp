#include "FormatTextLabel.h"
#include <Graphic/2D.h>

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
}

FormatTextLabel* FormatTextLabel::SetText(const std::string& newText) {
	text = newText;
	ParseText();
	return this;
}

FormatTextLabel* FormatTextLabel::AddText(const std::string& additionalText) {
	text += additionalText;
	ParseText();
	return this;
}

FormatTextLabel* FormatTextLabel::SetFormat(const std::string& key, const TextFormat& format) {
	_formats[key] = format;
	ParseText();
	return this;
}

void FormatTextLabel::ParseText() {
	_segments.clear();
	if (text.empty()) return;

	if (_fontHandle == -1) {
		ApplyFont();
	}

	struct RawSegment {
		std::string text;
		unsigned int color;
		bool underline = false;
	};
	std::vector<RawSegment> rawSegments;

	size_t i = 0;
	while (i < text.size()) {
		size_t nextOpen = text.find('{', i);
		if (nextOpen == std::string::npos) {
			rawSegments.push_back({ text.substr(i), foregroundColor, false });
			break;
		}
		if (nextOpen > i) {
			rawSegments.push_back({ text.substr(i, nextOpen - i), foregroundColor, false });
		}
		size_t closePos = text.find('}', nextOpen);
		if (closePos == std::string::npos) {
			rawSegments.push_back({ text.substr(nextOpen), foregroundColor, false });
			break;
		}

		std::string key = text.substr(nextOpen + 1, closePos - nextOpen - 1);
		if (_formats.contains(key)) {
			rawSegments.push_back({ _formats[key].text, _formats[key].color, true });
		}
		else {
			rawSegments.push_back({ "{" + key + "}", foregroundColor, false });
		}
		i = closePos + 1;
	}

	int lineSpacing = _fontSize;
	if (_fontHandle != -1) {
		int space = GetFontLineSpaceToHandle(_fontHandle);
		if (space > 0) lineSpacing = space;
	}

	int currentLine = 0;
	int currentLineWidth = 0;

	for (const auto& raw : rawSegments) {
		const std::string& segText = raw.text;
		std::string currentSegmentText;
		int segmentStartX = currentLineWidth;
		int segmentLine = currentLine;

		for (size_t index = 0; index < segText.size(); ) {
			if (segText[index] == '\r' || segText[index] == '\n') {
				if (not currentSegmentText.empty()) {
					_segments.push_back({
						currentSegmentText,
						Vector2D(static_cast<float>(segmentStartX), static_cast<float>(segmentLine * lineSpacing)),
						raw.color,
						raw.underline
					});
					currentSegmentText.clear();
				}

				if (segText[index] == '\r' && index + 1 < segText.size() && segText[index + 1] == '\n') {
					++index;
				}
				++index;

				currentLine++;
				currentLineWidth = 0;
				segmentStartX = 0;
				segmentLine = currentLine;
				continue;
			}

			const size_t charLen = GetCharacterLength(segText, index);
			const std::string character = segText.substr(index, charLen);

			int charWidth = 0;
			if (_fontHandle != -1) {
				charWidth = GetDrawNStringWidthToHandle(
					character.c_str(),
					static_cast<int>(character.size()),
					_fontHandle,
					FALSE
				);
			}

			if (_width > 0 && currentLineWidth > 0 && (currentLineWidth + charWidth > _width)) {
				if (not currentSegmentText.empty()) {
					_segments.push_back({
						currentSegmentText,
						Vector2D(static_cast<float>(segmentStartX), static_cast<float>(segmentLine * lineSpacing)),
						raw.color,
						raw.underline
					});
					currentSegmentText.clear();
				}

				currentLine++;
				currentLineWidth = 0;
				segmentStartX = 0;
				segmentLine = currentLine;
			}

			if (currentSegmentText.empty()) {
				segmentStartX = currentLineWidth;
				segmentLine = currentLine;
			}

			currentSegmentText += character;
			currentLineWidth += charWidth;
			index += charLen;
		}

		if (not currentSegmentText.empty()) {
			_segments.push_back({
				currentSegmentText,
				Vector2D(static_cast<float>(segmentStartX), static_cast<float>(segmentLine * lineSpacing)),
				raw.color,
				raw.underline
			});
		}
	}
}

void FormatTextLabel::_Draw() const {
	if (_fontHandle == -1) return;

	const auto pos = GetDrawPosition();
	const auto s = GetDrawScale();

	for (const auto& segment : _segments) {
		DrawExtendNStringToHandle(
			static_cast<int>(pos.x + segment.position.x * s.x),
			static_cast<int>(pos.y + segment.position.y * s.y),
			s.x, s.y,
			segment.text.c_str(),
			static_cast<int>(segment.text.size()),
			segment.color,
			_fontHandle
		);
		if (segment.underline) {
			int underlineY = static_cast<int>(pos.y + (segment.position.y + GetFontLineSpaceToHandle(_fontHandle) - 2) * s.y);
			int underlineStartX = static_cast<int>(pos.x + segment.position.x * s.x);
			int underlineEndX = underlineStartX + static_cast<int>(GetDrawNStringWidthToHandle(segment.text.c_str(), static_cast<int>(segment.text.size()), _fontHandle, FALSE) * s.x);
			NovotRenderer::DrawLine(Vector2D(underlineStartX, underlineY), Vector2D(underlineEndX, underlineY), segment.color, static_cast<int>(4 * s.y));
		}
	}
}