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
	char keyState[256] = {}, prevKeyState[256] = {};
	_KeyMapping() : mapping(){}
	void Update();
	template <typename... Args>
	requires (std::same_as<Args, int> && ...)
	void RegisterMapping(const std::string& action, Args... keys) {
		for (int key : {keys...}) {
			mapping[action].push_back(key);
		}
	}
	bool IsActionPressing(const std::string& action);
	bool IsActionJustPressed(const std::string& action);
	void StartDetect() {
		isDetected = false;
	}
	bool IsDetected() const {
		return isDetected;
	}
};

#define KeyMapping _KeyMapping::GetInstance()
