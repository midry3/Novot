#pragma once
#include "TextLabel.h"

class AnimatedTextLabel : public TextLabel {
protected:
	struct Character {
		std::string text;
		Vector2D position;
		float offset = 0.0f;
	};

	std::vector<Character> _characters;
	double _elapsedTime = 0.0;
	double _bounceHeight = 12.0;
	double _bounceUpTime = 0.12;
	double _bounceDownTime = 0.18;
	double _characterDelay = 0.04;
	bool _loop = true;

	void _Load() override;
	void _Update(float dt) override;
	void _Draw() const override;
	void _Release() override;
	void _RebuildCharacters();

public:
	AnimatedTextLabel* BeginSetting();
	AnimatedTextLabel* EndSetting();
	AnimatedTextLabel* SetText(const std::string& newText) override;
	AnimatedTextLabel* AddText(const std::string& additionalText) override;
	AnimatedTextLabel* SetFont(const std::string fontName);
	AnimatedTextLabel* SetFontSize(int size);
	AnimatedTextLabel* SetFontThickness(int thickness);
	AnimatedTextLabel* SetWidth(int width);

	AnimatedTextLabel* SetBounceHeight(double height);
	AnimatedTextLabel* SetBounceTime(double upTime, double downTime);
	AnimatedTextLabel* SetCharacterDelay(double delay);
	AnimatedTextLabel* SetLoop(bool loop);
};
