#pragma once
#include <string>

class NumberingText {
private:
	std::string _text;
public:
	NumberingText() = default;
	NumberingText(const std::string& text) : _text(text){}

	NumberingText operator+(int n) const;

	const std::string& str() const {
		return _text;
	}

	const char* c_str() const {
		return _text.c_str();
	}

	operator const std::string& () const {
		return _text;
	}
};

NumberingText operator""_N(const char* str, std::size_t len);