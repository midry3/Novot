#include "StateMachine.h"

State& State::Block(const int state) {
	_blackList.push_back(state);
	return *this;
}

State& State::Allow(const int state) {
	_whiteList.push_back(state);
	return *this;
}

bool State::IsAccept(const int state) const {
	if (_whiteList.empty()) {
		for (const auto& blockedState : _blackList) {
			if (blockedState == state) {
				return false;
			}
		}
		return true;
	}
	else {
		for (const auto& allowedState : _whiteList) {
			if (allowedState == state) {
				return true;
			}
		}
		return false;
	}
}