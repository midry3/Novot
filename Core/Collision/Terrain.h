#pragma once
#include "Collision.h"
#include "CollisionManager.h"
#include <list>
#include <memory>

class Terrain : public Collision {
protected:
	std::list<std::shared_ptr<Collision>> collisions;
public:
	Terrain() {
		isPassable = false;
	}
	std::shared_ptr<Collision> NewCollision() {
		auto col = CollisionManager.New();
		collisions.push_back(col);
		return col;
	}
};