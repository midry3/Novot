#include "MouseCollision.h"

#include <Graphic/2D.h>
#include <NovotCore.h>
#include <algorithm>
#include <Camera.h>
#include <Mouse/Mouse.h>

void MouseCollision::_Update(float dt) {
	if (!Mouse.AreEventsEnabled()) {
		_isMouseWithin = false;
		return;
	}
	if (not _enabled || Mouse.IsAlreadyHandled() || vertices.empty()) {
		if (_isMouseWithin) {
			_isMouseWithin = false;
			Exited.Emit(Mouse.GetMouseEvent());
		}
		return;
	}
	auto event = Mouse.GetMouseEvent();
	if (IsWithin(event.position)) {
		if (not _isMouseWithin) {
			Entered.Emit(event);
		}
		_isMouseWithin = true;
		CallCallback(event);
		if (_stop) {
			Mouse.Handled();
		}
	}
	else if (_isMouseWithin) {
		_isMouseWithin = false;
		Exited.Emit(event);
	}
}

std::vector<Vector2D> MouseCollision::GetDrawVertices() const {
	std::vector<Vector2D> res;
	for (auto v : vertices) {
		const auto worldV = Vertex2Global(v);
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
	auto verts = GetDrawVertices();
	if (verts.size() < 3) return false;
	bool inside = false;
	for (size_t i = 0, j = verts.size() - 1; i < verts.size(); j = i++) {
		if (((point.y < verts[i].y) != (point.y < verts[j].y)) &&
			(point.x < (verts[j].x - verts[i].x) * (point.y - verts[i].y) / (verts[j].y - verts[i].y) + verts[i].x)) {
			inside = not inside;
		}
	}
	return inside;
}

void MouseCollision::CallCallback(const MouseEvent& event) {
	switch (event.button) {
	case MouseButton::Left:
		if (event.state == MouseButtonState::Pressed) {
			LeftPressed.Emit(event);
		}
		else if (event.state == MouseButtonState::Released) {
			LeftReleased.Emit(event);
		}
		break;
	case MouseButton::Middle:
		if (event.state == MouseButtonState::Pressed) {
			MiddlePressed.Emit(event);
		}
		else if (event.state == MouseButtonState::Released) {
			MiddleReleased.Emit(event);
		}
		break;
	case MouseButton::Right:
		if (event.state == MouseButtonState::Pressed) {
			RightPressed.Emit(event);
		}
		else if (event.state == MouseButtonState::Released) {
			RightReleased.Emit(event);
		}
		break;
	case MouseButton::None:
		if (event.scrollOffset.Length() != 0) {
			Scrolled.Emit(event);
		}
		else if (event.state == MouseButtonState::Moved) {
			Moved.Emit(event);
		}
		break;
	}
}

#ifdef _DEBUG
void MouseCollision::_Load() {
	alpha = 120;
}

void MouseCollision::_Draw() const {
	if (!PropertyManager.IsDebugMode() || !PropertyManager.IsCollisionDebugVisible()) return;
	auto verts = GetDrawVertices();
	if (verts.size() < 2) return;
	NovotRenderer::DrawPolygon(verts, DEBUG_LINE_COLOR, true);
}
#endif
