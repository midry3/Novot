#pragma once
#include <MathUtils.h>
#include <Scene.h>
#include <vector>
#include <functional>

class Collision : public Scene {
protected:
	std::function<void(const Collision&)> onCollideCallback;
	std::vector<Vector2D> vertices;
	Vector2D scale;
	std::vector<int> layers;
	bool isPassable;
	bool enabled = true;
	int id = 0;
	void _Draw() const override;
	void _Update(float dt) override {};
	void _Load() override {};
	void _Release() override {};
public:
	Collision() : scale(1, 1), isPassable(false) {}
	~Collision() = default;
	void Enable() {
		enabled = true;
	}
	void Disable() {
		enabled = false;
	}
	bool IsEnabled() const {
		return enabled;
	}
	void AddLayer(int layer) {
		layers.push_back(layer);
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
	Vector2D Transform(const Vector2D& v) const {
		return v * scale + GetGlobalPosition();
	}
	std::vector<Vector2D> GetEdges() const;
	bool CanCollideWith(const Collision& other) const;
	bool IsColliding(const Collision& other) const;
	Vector2D GetNearestEdge(const Vector2D& point) const;
	void SetPassable(bool passable) {
		isPassable = passable;
	}
	bool CanPass() const {
		return isPassable;
	}
	void SetID(int newID) {
		id = newID;
	}
	int GetID() const {
		return id;
	}
	void OnCollide(const std::function<void(const Collision&)>& callback) {
		onCollideCallback = callback;
	}
	void CollideCallback(const Collision& other) const {
		if (onCollideCallback) {
			onCollideCallback(other);
		}
	}
	bool operator==(const Collision& other) const {
		return id == other.GetID();
	}
};