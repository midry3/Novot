#pragma once
#include <Scene.h>
#include <Math/Polygon.h>
#include <Backend/Backend.h>

class ColorPolygon : public Scene {
protected:
	PolygonInfo _polygon;
	void _Draw() const override;
public:
	unsigned int color = CoreBackend::Drawing::Color(255, 255, 255);

	ColorPolygon* SetColor(unsigned int c) {
		color = c;
		return this;
	}

	ColorPolygon* AddVertex(const Vector2D& vertex) {
		_polygon.AddVertex(vertex);
		return this;
	}

	template <typename... T>
		requires (std::same_as<Vector2D, T> && ...)
	ColorPolygon* AddVertices(T... vertices) {
		_polygon.AddVertices(vertices...);
		return this;
	}

	ColorPolygon* ClearVertices() {
		_polygon.Clear();
		return this;
	}

	ColorPolygon* SetVertices(const std::vector<Vector2D>& vertices) {
		_polygon.SetVertices(vertices);
		return this;
	}
};