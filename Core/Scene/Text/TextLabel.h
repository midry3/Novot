#pragma once
#include <Scene.h>
#include <Collision/MouseCollision.h>
#include <Backend/Backend.h>

class TextLabel : public Scene {
protected:
	int _screenHandle = -1;
	int _fontHandle = -1;
	int _fontSize = 24;
	int _fontThickness = 3;
	int _yScrollOffset = 0;
	String _fontName = "", _text = "";
	int _width = -1, _height = -1;
	mutable Vector2D _rawSizeCache;
	bool _isNowSetting = false;

	SharedPtr<MouseCollision> _mouseCollision;

	void _Load() override;
	void _Draw() const override;
	void _Release() override;
public:
	Signal<const String&> TextChanged;

	static String DefaultFont;
	unsigned int foregroundColor = CoreBackend::Drawing::Color(255, 255, 255);

	const Vector2D& GetRawSize() const override;

	TextLabel* UpdateDrawScreen();
	TextLabel* AdjustMouseCollisionArea();

	TextLabel* BeginSetting() {
		_isNowSetting = true;
		return this;
	}

	TextLabel* EndSetting() {
		_isNowSetting = false;
		ApplyFont();
		return this;
	}

	virtual TextLabel* SetText(const std::string& newText) {
		_text = newText;
		TextChanged.Emit(_text);
		return this;
	}

	virtual TextLabel* AddText(const std::string& additionalText) {
		_text += additionalText;
		TextChanged.Emit(_text);
		return this;
	}

	TextLabel* SetForegroundColor(unsigned int color) {
		foregroundColor = color;
		return this;
	}

	TextLabel* SetForegroundColor(const Vector3D& color) {
		foregroundColor = color.ToColor();
		return this;
	}

	void ApplyFont() {
		if (_isNowSetting) return;

		if (_fontHandle != -1) {
			CoreBackend::Drawing::DeleteFont(_fontHandle);
		}
		_fontHandle = CoreBackend::Drawing::CreateTextFont(_fontName.c_str(), _fontSize, _fontThickness);
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
		UpdateDrawScreen();
		AdjustMouseCollisionArea();
		return this;
	}

	TextLabel* SetHeight(int height) {
		_height = height;
		UpdateDrawScreen();
		AdjustMouseCollisionArea();
		return this;
	}

	void Clear() {
		_text.clear();
	}
};
