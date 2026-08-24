#pragma once
#include <MathUtils.h>
#include <Scene.h>
#include <Mouse/MouseEvent.h>
#include <functional>
#include <vector>

class MouseCollision : public Scene {
protected:
	const unsigned int DEBUG_LINE_COLOR = GetColor(0, 255, 0);
	std::map<MouseButton, std::map<MouseButtonState, std::function<void(const MouseEvent&)>>> onCollideCallbacks;
	std::vector<Vector2D> vertices;
	Vector2D scale;
	bool _enabled = true;
	bool _stop = true;
	void _Draw() const override;
	void _Update(float dt) override {};
	void _Load() override {};
	void _Release() override {};
public:
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
		return v * scale + GetGlobalPosition();
	}
	std::vector<Vector2D> GetEdges() const;
	Vector2D GetNearestEdge(const Vector2D& point) const;
	void RegisterCallback(MouseButton button, MouseButtonState state, const std::function<void(const MouseEvent&)>& callback) {
		onCollideCallbacks[button][state] = callback;
	}
	void CallCallback(const MouseEvent& event) const {
		auto it = onCollideCallbacks.find(event.button);
		if (it != onCollideCallbacks.end()) {
			auto stateIt = it->second.find(event.state);
			if (stateIt != it->second.end()) {
				stateIt->second(event);
			}
		}
	}
};