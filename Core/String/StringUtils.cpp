#include "StringUtils.h"

namespace StringUtils {
	std::string ToLower(const std::string& str) {
		std::string result = str;
		for (char& c : result) {
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}
		return result;
	}

	std::string ToUpper(const std::string& str) {
		std::string result = str;
		for (char& c : result) {
			c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
		}
		return result;
	}

	std::string Trim(const std::string& str) {
		size_t start;
		size_t end;
		for (start = 0; start < str.size() && std::isspace(static_cast<unsigned char>(str[start])); ++start);
		for (end = str.size(); end > start && std::isspace(static_cast<unsigned char>(str[end - 1])); --end);
		return str.substr(start, end - start);
	}
}