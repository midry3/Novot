#pragma once
#include <MathUtils.h>
#include <Scene.h>
#include <Mouse/MouseEvent.h>
#include <vector>

class MouseCollision : public Scene {
protected:
#ifdef _DEBUG
	const unsigned int DEBUG_LINE_COLOR = GetColor(100, 255, 100);
	void _Draw() const override;
	void _Load() override;
#endif
	std::vector<Vector2D> vertices;
	Vector2D scale;
	bool _enabled = true;
	bool _stop = true;
public:
	Signal<const MouseEvent&> LeftPressed, LeftReleased, MiddlePressed, MiddleReleased, RightPressed, RightReleased;

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
	void AddVertex(const Vector2D& vertex);
	std::vector<Vector2D> GetVertices() const {
		std::vector<Vector2D> res;
		for (auto v : vertices) {
			res.emplace_back(Transform(v));
		}
		return res;
	}
	void ClearVertices() {
		vertices.clear();
	}
	Vector2D Transform(const Vector2D& v) const {
		return v * scale + GetGlobalPosition() - Camera.GetPoint();
	}
	std::vector<Vector2D> GetEdges() const;
	Vector2D GetNearestEdge(const Vector2D& point) const;
	void CallCallback(const MouseEvent& event) {
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
		}
	}
};