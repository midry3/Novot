#pragma once
#include <Math/Vector2D.h>
#include <VisualDebugScene.h>
#include <Mouse/MouseEvent.h>
#include <vector>
#include <Scene/Texture/Sprite2D.h>
#include <Camera.h>
#include <Backend/Backend.h>
#include <cmath>

class MouseCollision : public VisualDebugScene {
protected:
#ifdef _DEBUG
	const unsigned int DEBUG_LINE_COLOR = CoreBackend::Drawing::Color(100, 255, 100);
	void _Draw() const override;
	void _Load() override;
#endif
	std::vector<Vector2D> vertices;
	Vector2D scale;
	bool _enabled = true;
	bool _stop = true;
	bool _isMouseWithin = false;
	void _Update(float dt) override;
public:
	Signal<const MouseEvent&> LeftPressed, LeftReleased, MiddlePressed, MiddleReleased, RightPressed, RightReleased, Moved, Scrolled, Entered, Exited;

	MouseCollision() : scale(1, 1) {}
	~MouseCollision() = default;
	bool IsWithin(const Vector2D& point) const;
	void Enable() {
		_enabled = true;
	}
	void Disable() {
		_enabled = false;
	}
	bool IsEnabled() const {
		return _enabled;
	}
	void SetStop(bool stop) {
		_stop = stop;
	}
	bool IsStop() const {
		return _stop;
	}
	void SetScale(const Vector2D& s) {
		scale = s;
	}
	void ChangeVertice(int index, const Vector2D& newVertex) {
		if (index >= 0 && index < static_cast<int>(vertices.size())) {
			vertices[index] = newVertex;
		}
	}
	template <typename... Args>
	requires (std::same_as<Args, Vector2D> && ...)
	MouseCollision* AddVertices(Args... vertex) {
		for (auto& v : { vertex... }) {
			vertices.emplace_back(v);
		}
		return this;
	}
	MouseCollision* AddVertices(std::vector<Vector2D> v) {
		for (const auto& vert : v) {
			vertices.emplace_back(vert);
		}
		return this;
	}
	std::vector<Vector2D> GetVertices() const {
		std::vector<Vector2D> res;
		for (auto v : vertices) {
			res.emplace_back(Vertex2Global(v));
		}
		return res;
	}
	std::vector<Vector2D> GetDrawVertices() const;
	void ClearVertices() {
		vertices.clear();
	}
	Vector2D Vertex2Global(const Vector2D& v) const {
		Vector2D targetV = v;
		const Vector2D s = GetScale();

		if (not parent.expired()) {
			auto p = parent.lock();
			Vector2D frameSize(0, 0);
			for (const auto& child : p->GetChildScenes()) {
				if (auto sprite = std::dynamic_pointer_cast<Sprite2D>(child)) {
					frameSize = sprite->GetRawSize();
					if (frameSize.x > 0 || frameSize.y > 0) break;
				}
			}
			if (s.x < 0) {
				targetV.x = (frameSize.x > 0) ? (frameSize.x - targetV.x) : -targetV.x;
			}
			if (s.y < 0) {
				targetV.y = (frameSize.y > 0) ? (frameSize.y - targetV.y) : -targetV.y;
			}
		}
		else {
			if (s.x < 0) targetV.x = -targetV.x;
			if (s.y < 0) targetV.y = -targetV.y;
		}

		const Vector2D scaledVertex(targetV.x * std::abs(s.x), targetV.y * std::abs(s.y));
		const double angle = GetRotation();
		const double cosine = std::cos(angle);
		const double sine = std::sin(angle);
		return GetGlobalPosition() + Vector2D(
			scaledVertex.x * cosine - scaledVertex.y * sine,
			scaledVertex.x * sine + scaledVertex.y * cosine
		);
	}
	std::vector<Vector2D> GetEdges() const;
	Vector2D GetNearestEdge(const Vector2D& point) const;
	void CallCallback(const MouseEvent& event);
};