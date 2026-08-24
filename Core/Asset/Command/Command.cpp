#include "Command.h"
#include <NovotCore.h>

int Command::Load() {
	std::vector<uint8_t> data = GetData();
	std::string content(data.begin(), data.end());
	std::istringstream iss(content);
	std::string line;
	int lineNumber = 0;
	while (std::getline(iss, line)) {
		lineNumber++;
		std::istringstream lineStream(line);
		std::string word;
		int columnNumber = 0;
		while (lineStream >> word) {
			columnNumber++;
			CommandType type = CommandType::STRING;
			if (std::all_of(
				word.begin(),
				word.end(),
				[](unsigned char c) {
					return std::isdigit(c);
				}
			)) {
				type = CommandType::NUMBER;
			}
			else if (word == "g") {
				type = CommandType::GRAVITY;
			}
			else if (word == "s") {
				type = CommandType::SLAM;
			}
			else if (word == "l") {
				type = CommandType::LEFT;
			}
			else if (word == "r") {
				type = CommandType::RIGHT;
			}
			else if (word == "u") {
				type = CommandType::UP;
			}
			else if (word == "d") {
				type = CommandType::DOWN;
			}
			else if (word == "b") {
				type = CommandType::BONE;
			}
			else if (word == "br") {
				type = CommandType::BONE_RECT;
			}
			else if (word == ";") {
				type = CommandType::TIME;
			}
			else if (word == "red") {
				type = CommandType::RED;
			}
			else if (word == "blue") {
				type = CommandType::BLUE;
			}
			tokens.push_back({ type, word, lineNumber, columnNumber });
		}
	}
	tokens.push_back({ CommandType::TIME, ";", -1, -1 });
	tokens.push_back({ CommandType::NUMBER, "1", -1, -1 });
	tokens.push_back({ CommandType::FIN, "", -1, -1 });
	return 0;
}

void Command::Release() {
	tokens.clear();
}