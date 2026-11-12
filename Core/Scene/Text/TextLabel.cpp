#include "TextLabel.h"
#include <Backend/Backend.h>
#include <Math/Utils.h>
#include <cmath>

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
			const int candidateWidth = CoreBackend::Drawing::MeasureText(
				candidate.c_str(),
				static_cast<int>(candidate.size()),
				fontHandle,
				0
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
	UpdateDrawScreen();

	_mouseCollision = CreateScene<MouseCollision>();

	_mouseCollision->Scrolled += [this](const MouseEvent& event) {
		if (_height <= 0) return;

		const auto& lim = static_cast<int>(GetRawSize().y);
		_yScrollOffset = Clamp(_yScrollOffset - static_cast<int>(event.scrollOffset.y * 2), -lim, lim);
		};

	AdjustMouseCollisionArea();

	TextChanged += [this](const String& newText) {
		UpdateDrawScreen();
		AdjustMouseCollisionArea();
		};
}

void  TextLabel::_Draw() const {
	if (_fontHandle == -1) {
		return;
	}
	const bool useDrawScreen = 0 <= _height and _screenHandle != -1;
	int screen = -1;
	if (useDrawScreen) {
		screen = CoreBackend::Drawing::GetCurrentDrawScreen();

		CoreBackend::Drawing::SetDrawScreen(_screenHandle);
		CoreBackend::Drawing::ClearDrawScreen();
	}

	int lineSpacing = CoreBackend::Drawing::FontLineSpace(_fontHandle);
	if (lineSpacing <= 0) {
		lineSpacing = _fontSize;
	}

	const auto s = GetDrawScale();
	const auto lines = WrapText(_text, _width, _fontHandle);
	for (size_t index = 0; index < lines.size(); ++index) {
		const auto linePosition = useDrawScreen
			? Vector2D(0, static_cast<double>(index * lineSpacing))
			: GetDrawPoint(Vector2D(0, static_cast<double>(index * lineSpacing)));
		CoreBackend::Drawing::RenderText(
			static_cast<int>(linePosition.x),
			static_cast<int>(linePosition.y) - _yScrollOffset,
			useDrawScreen ? 1.0 : s.x, useDrawScreen ? 1.0 : s.y,
			0, 0,
			GetRotation(),
			foregroundColor,
			_fontHandle,
			foregroundColor,
			0,
			lines[index].c_str()
		);
	}

	if (useDrawScreen) {
		CoreBackend::Drawing::SetDrawScreen(screen);
		const auto drawPosition = GetDrawPosition();
		const auto rawSize = GetRawSize();

		CoreBackend::Drawing::DrawSprite(
			static_cast<int>(drawPosition.x + rawSize.x * std::abs(s.x) / 2.0),
			static_cast<int>(drawPosition.y + rawSize.y * std::abs(s.y) / 2.0),
			static_cast<int>(rawSize.x),
			static_cast<int>(rawSize.y),
			s.x, s.y,
			GetRotation(),
			_screenHandle,
			TRUE
		);
	}
}

void TextLabel::_Release() {
	if (_fontHandle != -1) {
		CoreBackend::Drawing::DeleteFont(_fontHandle);
		_fontHandle = -1;
	}
	if (_screenHandle != -1) {
		CoreBackend::Drawing::DeleteDrawScreen(_screenHandle);
		_screenHandle = -1;
	}
}

const Vector2D& TextLabel::GetRawSize() const {
	if (not (_height < 0)) {
		const int measuredWidth = _text.empty() or _fontHandle == -1
			? 0
			: CoreBackend::Drawing::MeasureText(_text.c_str(), static_cast<int>(_text.size()), _fontHandle);
		_rawSizeCache = Vector2D(not (_width <= 0) ? _width : measuredWidth, _height);
		return _rawSizeCache;
	}
	if (_text.empty() or _fontSize == 0 or _fontHandle == -1) {
		_rawSizeCache = Vec2D_0;
		return _rawSizeCache;
	}
	const auto lines = WrapText(_text, _width, _fontHandle);
	int lineSpacing = CoreBackend::Drawing::FontLineSpace(_fontHandle);
	if (lineSpacing <= 0) lineSpacing = _fontSize;
	int measuredWidth = _width;
	if (_width <= 0) {
		measuredWidth = 0;
		for (const auto& line : lines) {
			const int width = CoreBackend::Drawing::MeasureText(line.c_str(), static_cast<int>(line.size()), _fontHandle);
			if (measuredWidth < width) measuredWidth = width;
		}
	}
	_rawSizeCache = Vector2D(measuredWidth, static_cast<double>(lines.size() * lineSpacing));
	return _rawSizeCache;
}

TextLabel* TextLabel::UpdateDrawScreen() {
	if (_screenHandle != -1) {
		CoreBackend::Drawing::DeleteDrawScreen(_screenHandle);
	}
	if (_height < 0) return this;
	const auto s = GetRawSize();
	if (s.x <= 0 or s.y <= 0) return this;
	_screenHandle = CoreBackend::Drawing::CreateDrawScreen(static_cast<int>(s.x), static_cast<int>(s.y));
	return this;
}

TextLabel* TextLabel::AdjustMouseCollisionArea() {
	if (_mouseCollision) {
		_mouseCollision->ClearVertices();
		const double& w = Max(_width, static_cast<int>(GetRawSize().x));
		const double& h = Max(_height, static_cast<int>(GetRawSize().y));
		_mouseCollision->AddVertices({ {0, 0}, {w, 0}, {w, h}, {0, h} });
		_mouseCollision->SetPosition({ -w/2, -h/2 });
	}
	return this;
}
