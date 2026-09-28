#include "RayCast.h"

void RayCast::_Load() {
	Collision::_Load();
	AddVertices(Vector0);
	AddVertices(Vector0);
}

void RayCast::_Update(float dt) {
	Collision::_Update(dt);
	if (IsColliding()) {
		double nearestLength = 99999999, length;
		for (auto col : collidingWith) {
			if (auto ptr = col.lock()) {
				length = (ptr->GetGlobalPosition() - GetGlobalPosition()).Length();
				if (length < nearestLength) {
					_nearest = ptr->GetGlobalPosition();
					nearestLength = length;
				}
			}
		}
	}
}