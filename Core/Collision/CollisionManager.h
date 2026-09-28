#pragma once
#include "Collision.h"
#include <NovotCore.h>
#include <Singleton.h>
#include <list>
#include <vector>
#include <memory>

class _CollisionManager : public Singleton<_CollisionManager> {
private:
	std::list<std::weak_ptr<Collision>> collisions;
public:
	std::shared_ptr<Collision> New() {
		auto collision = std::make_shared<Collision>();
		collisions.push_back(collision);
		return collision;
	}

	std::vector<std::shared_ptr<Collision>> UpdateCollisions() {
		std::vector<std::shared_ptr<Collision>> res;
		std::list<std::weak_ptr<Collision>> aliveCollisions;
		for (const auto& collision : collisions) {
			if (auto c = collision.lock()) {
				res.push_back(c);
				aliveCollisions.push_back(collision);
			}
		}
		collisions = aliveCollisions;
		return res;
	}

	std::shared_ptr<Collision> GetColliding(const Collision& target) {
		if (not target.IsEnabled()) {
			return nullptr;
		}
		for (const auto& collision : UpdateCollisions()) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && collision->CheckColliding(target)) {
				return collision;
			}
		}
		return nullptr;
	}

	std::shared_ptr<Collision> GetCollidingWall(const Collision& target) {
		if (!target.IsEnabled()) {
			return nullptr;
		}
		for (const auto& collision : UpdateCollisions()) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && collision->CheckColliding(target) && !collision->CanPass()) {
				return collision;
			}
		}
		return nullptr;
	}

	std::list<std::shared_ptr<Collision>> GetAllColliding(const Collision& target) {
		std::list<std::shared_ptr<Collision>> res;
		if (not target.IsEnabled()) {
			return res;
		}
		for (const auto& collision : UpdateCollisions()) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && target.CheckColliding(*collision)) {
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
		for (const auto& collision : UpdateCollisions()) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && collision->CheckColliding(target) && !collision->CanPass()) {
				res.push_back(collision);
			}
		}
		return res;
	}

	bool IsColliding(const Collision& target) {
		if (!target.IsEnabled()) {
			return false;
		}
		for (const auto& collision : UpdateCollisions()) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && collision->CheckColliding(target)) {
				return true;
			}
		}
		return false;
	}

	bool IsCollidingWithWall(const Collision& target) {
		if (!target.IsEnabled()) {
			return false;
		}
		for (const auto& collision : UpdateCollisions()) {
			if (*collision == target) {
				continue;
			}
			if (collision->IsEnabled() && collision->CheckColliding(target) && !collision->CanPass()) {
				return true;
			}
		}
		return false;
	}

	void AddCollision(std::weak_ptr<Collision> collision) {
		collisions.push_back(collision);
	}

	void RemoveCollision(const std::shared_ptr<Collision>& collision) {
		collisions.remove_if([&collision](const std::weak_ptr<Collision>& wp) {
			return wp.lock() == collision;
		});
	}

};

#define CollisionManager _CollisionManager::GetInstance()