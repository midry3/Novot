#pragma once
#include <DxLib.h>
#include <Singleton.h>
#include <map>
#include <string>
#include <list>

enum DefautKeyMapping {
	KEY_SHIFT
};	

class _KeyMapping : public Singleton<_KeyMapping> {
private:
	std::map<int, std::list<int>> mapping;
	bool _isDetected = false;
public:
	char keyState[256] = {}, prevKeyState[256] = {};
	_KeyMapping() = default;
	~_KeyMapping() = default;
	void Update();
	template <typename... Args>
	requires (std::same_as<Args, int> && ...)
	void RegisterMapping(const int action, Args... keys) {
		for (int key : {keys...}) {
			mapping[action].push_back(key);
		}
	}
	bool IsActionPressing(const int action);
	bool IsActionJustPressed(const int action);
	void StartDetect() {
		_isDetected = false;
	}
	bool IsDetected() const {
		return _isDetected;
	}
};

#define KeyMapping _KeyMapping::GetInstance()
