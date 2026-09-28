#pragma once
#include <list>
#include <functional>

template<typename... Types>
class Slot {
protected:
	std::function<void(Types...)> _callback;
	int id;
public:
	bool once;
	Slot() = default;
	Slot(const std::function<void(Types...)>& callback, int id, bool once = false) : _callback(callback), id(id), once(once) {}
	~Slot() = default;

	int  GetID() const {
		return id;
	}

	void operator()(Types... args) const {
		if (_callback) {
			_callback(args...);
		}
	}
};

template<typename... Types>
class Signal {
protected:
	std::list<Slot<Types...>> _callbacks;
	int nextId = -1;
public:
	Signal() = default;
	~Signal() = default;
	int Connect(const std::function<void(Types...)>& callback, bool once = false) {
		_callbacks.emplace_back(callback, ++nextId, once);
		return nextId;
	}

	void Disconnect(int id) {
		_callbacks.remove_if([id](const Slot<Types...>& slot) {
			return slot.GetID() == id;
			});
	}

	void Emit(Types... args) {
		auto callbacksCopy = _callbacks;
		std::vector<int> disconnectIDs;
		for (const auto& callback : callbacksCopy) {
			callback(args...);
			if (callback.once) {
				disconnectIDs.push_back(callback.GetID());
			}
		}
		for (int id : disconnectIDs) {
			Disconnect(id);
		}
	}

	void operator+=(const std::function<void(Types...)>& callback) {
		Connect(callback);
	}

	void operator-=(int id) {
		Disconnect(id);
	}

	void operator()(Types... args) {
		Emit(args...);
	}
};