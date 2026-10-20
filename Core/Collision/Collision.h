#pragma once
#include <Math/Vector2D.h>
#include <VisualDebugScene.h>
#include <vector>
#include <Scene/Texture/Sprite2D.h>
#include <Camera.h>

class Collision : public VisualDebugScene {
protected:
	std::vector<int> layers, masks;
	std::list<std::weak_ptr<Collision>> collidingWith;
	bool enabled = true;
	bool _isColliding = false;
	bool _needsUpdate = true;
	void _Update(float dt) override;
#ifdef _DEBUG
	void _Draw() const override;
	void _Load() override;
	const unsigned int DEBUG_COLLIDING_COLOR = GetColor(220, 100, 100);
	const unsigned int DEBUG_NOT_COLLIDING_COLOR = GetColor(100, 100, 220);
	int _collidingCount = 0;
#else
	bool isPassable;
	std::vector<Vector2D> vertices;
#endif
public:
#ifdef _DEBUG
	bool isPassable;
	std::vector<Vector2D> vertices;
#endif
	Signal<const Collision&> CollideStarted, Colliding, CollideExited;

	Collision() : isPassable(false), _needsUpdate(true) {}
	~Collision() = default;
	Collision* Enable() {
		enabled = true;
		return this;
	}
	Collision* Disable() {
		enabled = false;
		return this;
	}
	bool IsEnabled() const {
		return enabled;
	}
	Collision* SetNeedsUpdate(bool needsUpdate) {
		_needsUpdate = needsUpdate;
		return this;
	}
	template <typename... Layers>
	requires (std::convertible_to<Layers, int> && ...)
	Collision* AddLayers(Layers... l) {
		(layers.push_back(l), ...);
		return this;
	}
	template <typename... Masks>
		requires (std::convertible_to<Masks, int> && ...)
	Collision* AddMasks(Masks... m) {
		(masks.push_back(m), ...);
		return this;
	}
	void ChangeVertice(int index, const Vector2D& newVertex) {
		if (index >= 0 && index < static_cast<int>(vertices.size())) {
			vertices[index] = newVertex;
		}
	}
	template <typename... Vertices>
	requires (std::same_as<Vertices, Vector2D> && ...)
	Collision* AddVertices(Vertices... v) {
		(vertices.emplace_back(v), ...);
		return this;
	}
	Collision* AddVertices(std::vector<Vector2D> v) {
		for (const auto& vert : v) {
			vertices.emplace_back(vert);
		}
		return this;
	}
	std::vector<Vector2D> GetVertices() const {
		std::vector<Vector2D> res;
		for (auto v : vertices) {
			res.emplace_back(Transform(v));
		}
		return res;
	}
	std::vector<Vector2D> GetDrawVertices() const;
	Vector2D Transform(const Vector2D& v) const {
		Vector2D s = GetScale();
		Vector2D pos = GetGlobalPosition();
		Vector2D targetV = v;

		if (!parent.expired()) {
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

		return pos + Vector2D(targetV.x * std::abs(s.x), targetV.y * std::abs(s.y));
	}
	std::vector<Vector2D> GetEdges() const;
	void ClearVertices() {
		vertices.clear();
	}
	bool CanCollideWith(const Collision& other) const;
	bool CheckColliding(const Collision& other) const;
	bool IsColliding() const {
		return _isColliding;
	}
	Vector2D GetNearestEdge(const Vector2D& point) const;
	Collision* SetPassable(bool passable) {
		isPassable = passable;
		return this;
	}
	bool CanPass() const {
		return isPassable;
	}
	bool operator==(const Collision& other) const {
		return GetID() == other.GetID();
	}
};