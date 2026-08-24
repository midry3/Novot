#pragma once
#include <DxLib.h>
#include <Singleton.h>
#include <map>
#include <string>
#include <list>

class _KeyMapping : public Singleton<_KeyMapping> {
private:
	std::map<std::string, std::list<int>> mapping;
	bool isDetected = false;
public:
	char keyState[256];
	_KeyMapping() : mapping(){}
	void Update();
	void RegisterMapping(const std::string& action, const std::list<int>& keys) {
		mapping[action] = keys;
	}
	void RegisterMapping(const std::string& action, int key);
	bool IsActionPressing(const std::string& action);
	void StartDetect() {
		isDetected = false;
	}
	bool IsDetected() const {
		return isDetected;
	}
};

#define KeyMapping _KeyMapping::GetInstance()
