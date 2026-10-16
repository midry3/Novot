#include "Json.h"
#include <fstream>

int Json::Load() {
	std::ifstream(file);
	_json = json::parse(file);
	return 0;
}

void Json::Release() {

}