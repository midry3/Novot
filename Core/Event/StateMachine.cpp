#include "StateMachine.h"

State& State::Block(const int state) {
	_blackList.push_back(state);
	return *this;
}

bool State::CanTransTo(const int state) const {
	for (const auto& blockedState : _blackList) {
		if (blockedState == state) {
			return false;
		}
	}
	return true;
}