#pragma once
#include <sstream>
#include <algorithm>
#include <Asset/Asset.h>
#include <vector>

enum class CommandType {
	FIN,
	STRING,
	NUMBER,
	RED,
	BLUE,
	TIME,
	GRAVITY,
	SLAM,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	BONE,
	BONE_RECT
};

struct Token {
	CommandType type;
	std::string value;
	int line;
	int column;
};

class Command : public Asset {
private:
	std::vector<Token> tokens;
	int currentIndex = 0;
public:
	Command() = default;
	Command(const std::string& file) : Asset(file) {}
	~Command() = default;

	int Load() override;
	void Release() override;

	Token GetToken(int index) const {
		if (index < 0 || index >= tokens.size()) {
			return Token{ CommandType::FIN, "", -1, -1 };
		}
		return tokens[index];
	}
	Token NextToken() {
		if (HasNextToken()) {
			return GetToken(currentIndex++);
		}
		return Token{ CommandType::FIN, "", -1, -1 };
	}
	bool HasNextToken() const {
		return currentIndex < tokens.size();
	}
};