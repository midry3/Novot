#pragma once
#include <Scene.h>

template <typename T>
	requires std::derived_from<T, Scene>
class ObjectPool {
private:
	Vector<SharedPtr<T>> _availables;
public:
	ObjectPool() = default;

	[[nodiscard]] size_t GetTotalCount() const {
		return _availables.size();
	}

	[[nodiscard]] size_t GetAvailableCount() const {
		return _availables.size();
	}

	[[nodiscard]] bool CanGet() const {
		return not _availables.empty();
	}

	void Add(const SharedPtr<T>& obj) {
		if (not obj) {
			return;
		}
		obj->Disable();
		_availables.push_back(obj);
	}

	void operator<<(const SharedPtr<T>& obj) {
		Add(obj);
	}

	SharedPtr<T> Get() {
		if (not CanGet()) {
			return nullptr;
		}
		auto obj = _availables.back();
		_availables.pop_back();
		obj->Enable();
		return obj;
	}
};