#include <DxLib.h>
#include <NovotCore.h>
#include <algorithm>
#include "Collision.h"

void Collision::AddVertex(const Vector2D& vertex) {
	vertices.emplace_back(vertex);
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
	for (int layer : layers) {
		for (int otherLayer : other.layers) {
			if (layer == otherLayer) {
				return true;
			}
		}
	}
	return false;
}

bool Collision::IsColliding(const Collision& other) const {
	if (vertices.size() == 0 || other.vertices.size() == 0 || !CanCollideWith(other)) return false;

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

void Collision::_Draw() const {
	if (!PropertyManager.IsDebugMode()) return;
	auto verts = GetVertices();
	if (verts.size() < 2) return;
	auto camera = Camera.GetPoint();
	auto it = verts.begin();
	auto prev = it;
	it++;
	for (; it != verts.end(); it++, prev++) {
		DrawLine(static_cast<int>(prev->x) - camera.x, static_cast<int>(prev->y) - camera.y, static_cast<int>(it->x) - camera.x, static_cast<int>(it->y) - camera.y, GetColor(255, 0, 0));
	}
	DrawLine(static_cast<int>(verts.back().x) - camera.x, static_cast<int>(verts.back().y) - camera.y, static_cast<int>(verts.front().x) - camera.x, static_cast<int>(verts.front().y) - camera.y, GetColor(255, 0, 0));
}