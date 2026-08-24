#include "Object.h"

void Object::_Draw() const {

}

void Object::_Load() {

}

void Object::_Release() {

}

void Object::_Update(float dt) {

}

void Object::MoveAndSlide(float dt) {
	velocity += acceleration * dt;
	position += velocity * dt;
	auto gpos = GetGlobalPosition();
	auto col = CollisionManager.GetAllCollidingWall(*collision);
	if (col.size() > 0) {
		if (col.size() > 1) {
			position = prePosition;
			velocity = Vector0;
		}
		else {
			auto e = col[0]->GetNearestEdge(gpos);
			if (e.x == 0) {
				position.x = prePosition.x;
				velocity.x = 0.0f;
			}
			else {
				position.y = prePosition.y;
				velocity.y = 0.0f;
			}
		}
	}
	for (const auto& c : CollisionManager.GetAllColliding(*collision)) {
		c->CollideCallback(*collision);
	}
	prePosition = position;
	preGlobalPosition = gpos;
}