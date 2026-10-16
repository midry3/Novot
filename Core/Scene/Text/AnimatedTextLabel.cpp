#include "AnimatedTextLabel.h"
#include <Graphic/2D.h>
#include <cmath>

namespace {
	size_t GetCharacterLength(const std::string& value, size_t index) {
		if (index >= value.size()) return 0;
		const unsigned char character = static_cast<unsigned char>(value[index]);

		if (character < 0x80) return 1;
		if (character >= 0xC2 && character <= 0xDF) {
			return index + 1 < value.size() ? 2 : 1;
		}
		if (character >= 0xE0 && character <= 0xEF) {
			return index + 2 < value.size() ? 3 : value.size() - index;
		}
		if (character >= 0xF0 && character <= 0xF4) {
			return index + 3 < value.size() ? 4 : value.size() - index;
		}
		if ((character >= 0x81 && character <= 0x9F) ||
			(character >= 0xE0 && character <= 0xFC)) {
			return index + 1 < value.size() ? 2 : 1;
		}
		return 1;
	}
}

void AnimatedTextLabel::_Load() {
	TextLabel::_Load();
	_RebuildCharacters();
}

void AnimatedTextLabel::_Update(float dt) {
	_elapsedTime += dt;

	const double bounceDuration = _bounceUpTime + _bounceDownTime;
	if (!_loop || _characters.empty() || bounceDuration <= 0.0) return;

	const double cycleDuration = _characterDelay * _characters.size() + bounceDuration;
	if (cycleDuration > 0.0 && cycleDuration <= _elapsedTime) {
		_elapsedTime = std::fmod(_elapsedTime, cycleDuration);
	}
}

void AnimatedTextLabel::_Release() {
	_characters.clear();
	TextLabel::_Release();
}

void AnimatedTextLabel::_RebuildCharacters() {
	_characters.clear();
	_elapsedTime = 0.0;
	if (_fontHandle == -1 || text.empty()) return;

	const int lineSpacing = GetFontLineSpaceToHandle(_fontHandle) > 0
		? GetFontLineSpaceToHandle(_fontHandle)
		: _fontSize;
	int line = 0;
	int lineWidth = 0;

	for (size_t index = 0; index < text.size();) {
		if (text[index] == '\r' || text[index] == '\n') {
			if (text[index] == '\r' && index + 1 < text.size() && text[index + 1] == '\n') {
				++index;
			}
			++index;
			++line;
			lineWidth = 0;
			continue;
		}

		const size_t characterLength = GetCharacterLength(text, index);
		const std::string character = text.substr(index, characterLength);
		const int characterWidth = GetDrawNStringWidthToHandle(
			character.c_str(), static_cast<int>(character.size()), _fontHandle, FALSE
		);
		if (_width > 0 && lineWidth > 0 && lineWidth + characterWidth > _width) {
			++line;
			lineWidth = 0;
		}

		_characters.push_back({
			character,
			Vector2D(static_cast<float>(lineWidth), static_cast<float>(line * lineSpacing))
		});
		lineWidth += characterWidth;
		index += characterLength;
	}
}

void AnimatedTextLabel::_Draw() const {
	if (_fontHandle == -1) return;

	const auto position = GetDrawPosition();
	const auto currentScale = GetScale();
	const double bounceDuration = _bounceUpTime + _bounceDownTime;
	const double cycleDuration = _characterDelay * _characters.size() + bounceDuration;

	for (size_t index = 0; index < _characters.size(); ++index) {
		auto& character = _characters[index];
		double offset = 0.0;
		const double characterTime = _elapsedTime - _characterDelay * index;
		if (characterTime >= 0.0 && bounceDuration > 0.0) {
			const double time = _loop && cycleDuration > 0.0
				? std::fmod(characterTime, cycleDuration)
				: characterTime;
			if (time < _bounceUpTime) {
				offset = -_bounceHeight * (time / _bounceUpTime);
			}
			else if (time < bounceDuration && _bounceDownTime > 0.0) {
				offset = -_bounceHeight *
					(1.0 - (time - _bounceUpTime) / _bounceDownTime);
			}
		}

		DrawExtendNStringToHandle(
			static_cast<int>(position.x + character.position.x * currentScale.x),
			static_cast<int>(position.y + (character.position.y + offset) * currentScale.y),
			currentScale.x, currentScale.y,
			character.text.c_str(), static_cast<int>(character.text.size()),
			foregroundColor, _fontHandle
		);
	}
}

AnimatedTextLabel* AnimatedTextLabel::BeginSetting() {
	TextLabel::BeginSetting();
	return this;
}

AnimatedTextLabel* AnimatedTextLabel::EndSetting() {
	TextLabel::EndSetting();
	if (_isLoaded) _RebuildCharacters();
	return this;
}

AnimatedTextLabel* AnimatedTextLabel::SetText(const std::string& newText) {
	TextLabel::SetText(newText);
	if (_isLoaded) _RebuildCharacters();
	return this;
}

AnimatedTextLabel* AnimatedTextLabel::AddText(const std::string& additionalText) {
	TextLabel::AddText(additionalText);
	if (_isLoaded) _RebuildCharacters();
	return this;
}

AnimatedTextLabel* AnimatedTextLabel::SetFont(const std::string fontName) {
	TextLabel::SetFont(fontName);
	if (_isLoaded) _RebuildCharacters();
	return this;
}

AnimatedTextLabel* AnimatedTextLabel::SetFontSize(int size) {
	TextLabel::SetFontSize(size);
	if (_isLoaded) _RebuildCharacters();
	return this;
}

AnimatedTextLabel* AnimatedTextLabel::SetFontThickness(int thickness) {
	TextLabel::SetFontThickness(thickness);
	if (_isLoaded) _RebuildCharacters();
	return this;
}

AnimatedTextLabel* AnimatedTextLabel::SetWidth(int width) {
	TextLabel::SetWidth(width);
	if (_isLoaded) _RebuildCharacters();
	return this;
}

AnimatedTextLabel* AnimatedTextLabel::SetBounceHeight(double height) {
	_bounceHeight = height;
	return this;
}

AnimatedTextLabel* AnimatedTextLabel::SetBounceTime(double upTime, double downTime) {
	_bounceUpTime = upTime;
	_bounceDownTime = downTime;
	return this;
}

AnimatedTextLabel* AnimatedTextLabel::SetCharacterDelay(double delay) {
	_characterDelay = delay;
	return this;
}

AnimatedTextLabel* AnimatedTextLabel::SetLoop(bool loop) {
	_loop = loop;
	return this;
}
