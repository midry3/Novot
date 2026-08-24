#pragma once
#include "Collision.h"
#include <NovotCore.h>
#include <Singleton.h>
#include <list>
#include <memory>

class _CollisionManager : public Singleton<_CollisionManager> {
private:
	std::list<std::shared_ptr<Collision>> collisions;
public:
	std::shared_ptr<Collision> New() {
		auto collision = std::make_shared<Collision>();
		collision->SetID(static_cast<int>(collisions.size()));
		collisions.push_back(collision);
		return collision;
	}

	std::shared_ptr<Collision> GetColliding(const Collision& target) {
		if (!target.IsEnabled()) {
			return nullptr;
		}
		for (const auto& collision : collisions) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && collision->IsColliding(target)) {
				return collision;
			}
		}
		return nullptr;
	}

	std::shared_ptr<Collision> GetCollidingWall(const Collision& target) {
		if (!target.IsEnabled()) {
			return nullptr;
		}
		for (const auto& collision : collisions) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && collision->IsColliding(target) && !collision->CanPass()) {
				return collision;
			}
		}
		return nullptr;
	}

	std::list<std::shared_ptr<Collision>> GetAllColliding(const Collision& target) {
		std::list<std::shared_ptr<Collision>> res;
		if (!target.IsEnabled()) {
			return res;
		}
		for (const auto& collision : collisions) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && collision->IsColliding(target)) {
				res.push_back(collision);
			}
		}
		return res;
	}

	std::vector<std::shared_ptr<Collision>> GetAllCollidingWall(const Collision& target) {
		std::vector<std::shared_ptr<Collision>> res;
		if (!target.IsEnabled()) {
			return res;
		}
		for (const auto& collision : collisions) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && collision->IsColliding(target) && !collision->CanPass()) {
				res.push_back(collision);
			}
		}
		return res;
	}

	bool IsColliding(const Collision& target) {
		if (!target.IsEnabled()) {
			return false;
		}
		for (const auto& collision : collisions) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && collision->IsColliding(target)) {
				return true;
			}
		}
		return false;
	}

	bool IsCollidingWithWall(const Collision& target) {
		if (!target.IsEnabled()) {
			return false;
		}
		for (const auto& collision : collisions) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && collision->IsColliding(target) && !collision->CanPass()) {
				DebuggerWriteLine("Collision detected with wall! Collision ID: " + std::to_string(collision->GetID()));
				return true;
			}
		}
		return false;
	}

	void AddCollision(std::shared_ptr<Collision> collision) {
		collisions.push_back(collision);
	}

	void RemoveCollision(std::shared_ptr<Collision> collision) {
		collisions.remove(collision);
	}

};

#define CollisionManager _CollisionManager::GetInstance()