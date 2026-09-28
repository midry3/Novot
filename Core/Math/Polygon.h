#pragma once
#include <vector>
#include <Collision/Collision.h>
#include "Vector2D.h"

class PolygonInfo {
	std::vector<Vector2D> _vertices;
public:
	PolygonInfo() = default;
	PolygonInfo(int n) {
		_vertices.resize(n);
	}
	~PolygonInfo() = default;

	PolygonInfo* AddVertex(const Vector2D& vertex) {
		_vertices.push_back(vertex);
		return this;
	}
	template <typename... T>
		requires (std::same_as<Vector2D, T> && ...)
	PolygonInfo* AddVertices(T... vertices) {
		(_vertices.push_back(vertices), ...);
		return this;
	}

	PolygonInfo* Clear() {
		_vertices.clear();
		return this;
	}

	PolygonInfo* SetVertices(const std::vector<Vector2D>& vertices) {
		_vertices = vertices;
		return this;
	}

	PolygonInfo* ChangeVertex(int index, const Vector2D& newVertex) {
		if (index >= 0 && index < static_cast<int>(_vertices.size())) {
			_vertices[index] = newVertex;
		}
		return this;
	}

	Collision CreateCollision() const {
		Collision collision;
		collision.AddVertices(_vertices);
		return collision;
	}

	PolygonInfo RelatedTo(const Vector2D O) const {
		PolygonInfo newPolygon;
		for (const auto& v : _vertices) {
			newPolygon.AddVertex(v + O);
		}
		return newPolygon;
	}

	std::vector<Vector2D> GetVertices() const {
		return _vertices;
	}
};