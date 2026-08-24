#include "KeyMapping.h"

void _KeyMapping::Update() {
	GetHitKeyStateAll(keyState);
}

void _KeyMapping::RegisterMapping(const std::string& action, int key) {
	if (mapping.find(action) == mapping.end()) {
		mapping[action] = std::list<int>();
	}
	mapping[action].push_back(key);
}
bool _KeyMapping::IsActionPressing(const std::string& action) {
	if (mapping.find(action) == mapping.end()) return false;
	for (int key : mapping[action]) {
		if (keyState[key]) {
			isDetected = true;
			return true;
		}
	}
	return false;
}