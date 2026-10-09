#include "NumberingText.h"

NumberingText NumberingText::operator+(int n) const {
	return NumberingText(_text + std::to_string(n));
}

NumberingText operator""_N(const char* str, std::size_t len) {
	return NumberingText(str);
}