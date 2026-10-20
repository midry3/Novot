#include <Graphic/2D.h>
#include <NovotCore.h>
#include <algorithm>
#include <Camera.h>
#include "Collision.h"
#include "CollisionManager.h"

void Collision::_Update(float dt) {
	if (not enabled || not _needsUpdate) return;
	std::list<std::weak_ptr<Collision>> currentCollisions;
	auto collisions = CollisionManager.GetAllColliding(*this);
#ifdef _DEBUG
	_collidingCount = collisions.size();
#endif
	if (collisions.size() == 0) {
		_isColliding = false;
	}
	else {
		for (auto col : collisions) {
			if (std::find_if(collidingWith.begin(), collidingWith.end(), [&col](const std::weak_ptr<Collision>& c) { return !c.expired() && c.lock() == col; }) == collidingWith.end()) {
				CollideStarted.Emit(*col);
			}
			currentCollisions.push_back(col);
			collidingWith.remove_if([&col](const std::weak_ptr<Collision>& c) { return !c.expired() && c.lock() == col; });
			Colliding.Emit(*col);
		}
	}
	for (auto remainingCol : collidingWith) {
		if (auto col = remainingCol.lock()) {
			CollideExited.Emit(*col);
		}
	}
	collidingWith = currentCollisions;
}

std::vector<Vector2D> Collision::GetDrawVertices() const {
	std::vector<Vector2D> res;
	for (auto v : vertices) {
		const auto worldV = Transform(v);
		if (IsIgnoringCamera()) {
			res.emplace_back(worldV);
		}
		else {
			res.emplace_back(Camera.WorldToScreen(worldV));
		}
	}
	Vector2D s = GetDrawScale();
	if ((s.x < 0) != (s.y < 0)) {
		std::reverse(res.begin(), res.end());
	}
	return res;
}

std::vector<Vector2D> Collision::GetEdges() const {
	std::vector<Vector2D> edges;
	if (vertices.size() < 2) return edges;
	auto v = GetVertices();
	auto it = v.begin();
	auto prev = it;
	it++;
	for (; it != v.end(); it++, prev++) {
		edges.emplace_back(*it - *prev);
	}
	edges.emplace_back(v.front() - v.back());
	return edges;
}

bool Collision::CanCollideWith(const Collision& other) const {
	for (int mask : masks) {
		for (int otherLayer : other.layers) {
			if (mask == otherLayer) {
				return true;
			}
		}
	}
	return false;
}

bool Collision::CheckColliding(const Collision& other) const {
	if (vertices.size() == 0 || other.vertices.size() == 0 || not CanCollideWith(other)) return false;

	auto edges = GetEdges();
	auto e2 = other.GetEdges();
	auto v2 = other.GetVertices();
	edges.insert(edges.end(), e2.begin(), e2.end());
	for (const auto& e : edges) {
		double min1 = std::numeric_limits<double>::infinity(), max1 = -std::numeric_limits<double>::infinity(), min2 = std::numeric_limits<double>::infinity(), max2 = -std::numeric_limits<double>::infinity();
		auto n = e.Perpendicular().Normalized();
		for (const auto& v : GetVertices()) {
			double p1 = n.Dot(v);
			if (p1 < min1) min1 = p1;
			if (p1 > max1) max1 = p1;
		}
		for (const auto& v : v2) {
			double p2 = n.Dot(v);
			if (p2 < min2) min2 = p2;
			if (p2 > max2) max2 = p2;
		}
		if (!IsColliding1D(min1, max1, min2, max2)) return false;
	}
	return true;
}

Vector2D Collision::GetNearestEdge(const Vector2D& point) const {
	auto verts = GetVertices();

	if (verts.size() < 2) {
		return Vector2D(0, 0);
	}

	Vector2D nearestEdge;
	double minDistSq = std::numeric_limits<double>::infinity();

	for (size_t i = 0; i < verts.size(); i++) {
		const auto& a = verts[i];
		const auto& b = verts[(i + 1) % verts.size()];

		Vector2D edge = b - a;
		Vector2D ap = point - a;

		double lenSq = edge.Dot(edge);
		if (lenSq <= 0.0) continue;

		double t = ap.Dot(edge) / lenSq;
		t = std::clamp(t, 0.0, 1.0);

		Vector2D closest = a + edge * t;

		double distSq = (point - closest).Dot(point - closest);

		if (distSq < minDistSq) {
			minDistSq = distSq;
			nearestEdge = edge;
		}
	}

	return nearestEdge;
}

#ifdef _DEBUG
void Collision::_Load() {
	alpha = 120;
}

void Collision::_Draw() const {
	if (not PropertyManager.IsDebugMode() || not PropertyManager.IsCollisionDebugVisible()) return;
	auto verts = GetDrawVertices();
	if (verts.size() < 2) return;
	
	NovotRenderer::DrawPolygon(verts, _collidingCount > 0 ? DEBUG_COLLIDING_COLOR : DEBUG_NOT_COLLIDING_COLOR, true);
}
#endif
