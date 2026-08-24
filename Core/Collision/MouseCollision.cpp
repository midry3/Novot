#include "MouseCollision.h"

#include <DxLib.h>
#include <NovotCore.h>
#include <algorithm>

void MouseCollision::AddVertex(const Vector2D& vertex) {
	vertices.emplace_back(vertex);
}

std::vector<Vector2D> MouseCollision::GetEdges() const {
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

Vector2D MouseCollision::GetNearestEdge(const Vector2D& point) const {
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

bool MouseCollision::IsWithin(const Vector2D& point) const {
	auto verts = GetVertices();
	if (verts.size() < 3) return false;
	bool inside = false;
	for (size_t i = 0, j = verts.size() - 1; i < verts.size(); j = i++) {
		if (((verts[i].y > point.y) != (verts[j].y > point.y)) &&
			(point.x < (verts[j].x - verts[i].x) * (point.y - verts[i].y) / (verts[j].y - verts[i].y) + verts[i].x)) {
			inside = !inside;
		}
	}
	return inside;
}

void MouseCollision::_Draw() const {
	if (!PropertyManager.IsDebugMode()) return;
	auto verts = GetVertices();
	if (verts.size() < 2) return;
	auto camera = Camera.GetPoint();
	auto it = verts.begin();
	auto prev = it;
	it++;
	for (; it != verts.end(); it++, prev++) {
		DrawLine(static_cast<int>(prev->x) - camera.x, static_cast<int>(prev->y) - camera.y, static_cast<int>(it->x) - camera.x, static_cast<int>(it->y) - camera.y, DEBUG_LINE_COLOR);
	}
	DrawLine(static_cast<int>(verts.back().x) - camera.x, static_cast<int>(verts.back().y) - camera.y, static_cast<int>(verts.front().x) - camera.x, static_cast<int>(verts.front().y) - camera.y, DEBUG_LINE_COLOR);
}