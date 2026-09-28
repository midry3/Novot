#pragma once

#include "Vector2D.h"
#include "LineSegment.h"

#include <concepts>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>

template <typename T>
class GraphItem {
protected:
	T value;
	Vector2D position;

public:
	GraphItem()
		: value{}, position(0, 0) {}

	GraphItem(const T& value, const Vector2D& position)
		: value(value), position(position) {}

	~GraphItem() = default;


	T GetValue() const {
		return value;
	}

	Vector2D GetPosition() const {
		return position;
	}


	GraphItem<T>* SetValue(const T& newValue) {
		value = newValue;
		return this;
	}

	GraphItem<T>* SetPosition(const Vector2D& newPosition) {
		position = newPosition;
		return this;
	}

	bool operator==(const GraphItem<T>& other) const {
		return value == other.value && position == other.position;
	}
};


template <typename T>
class Graph {
protected:
	std::vector<GraphItem<T>> _vertices;

public:
	Graph() = default;
	~Graph() = default;

	Graph* AddVertex(const T& vertex) {
		_vertices.emplace_back(vertex, Vector2D(0, 0));
		return this;
	}

	Graph* AddVertex(const GraphItem<T>& vertex) {
		_vertices.push_back(vertex);
		return this;
	}

	template <typename... T2>
		requires (std::same_as<T, T2> && ...)
	Graph* AddVertices(const T2&... vertices) {
		(_vertices.emplace_back(vertices, Vector2D(0, 0)), ...);
		return this;
	}

	template <typename... T2>
		requires (std::same_as<GraphItem<T>, T2> && ...)
	Graph* AddVertices(const T2&... vertices) {
		(_vertices.push_back(vertices), ...);
		return this;
	}

	std::vector<GraphItem<T>> GetVertices() const {
		return _vertices;
	}

	bool Empty() const {
		return _vertices.empty();
	}

	void Clear() {
		_vertices.clear();
	}

	void RemoveAt(int index) {
		if (index >= 0 && index < static_cast<int>(_vertices.size())) {
			_vertices.erase(_vertices.begin() + index);
		}
	}

	GraphItem<T> Pop() {
		if (_vertices.empty()) {
			throw std::out_of_range("Graph is empty");
		}
		GraphItem<T> vertex = _vertices.back();
		_vertices.pop_back();
		return vertex;
	}

	GraphItem<T> Last() const {
		if (_vertices.empty()) {
			throw std::out_of_range("Graph is empty");
		}
		return _vertices.back();
	}

	void ChangeVertex(int index, const T& newVertex) {
		if (index >= 0 && index < static_cast<int>(_vertices.size())) {
			_vertices[index] = GraphItem<T>(newVertex, Vector2D(0, 0));
		}
	}


	void ChangeVertex(int index, const GraphItem<T>& newVertex) {
		if (index >= 0 && index < static_cast<int>(_vertices.size())) {
			_vertices[index] = newVertex;
		}
	}


	void InsertVertex(int index, const T& vertex) {
		if (index >= 0 && index <= static_cast<int>(_vertices.size())) {
			_vertices.insert(_vertices.begin() + index, GraphItem<T>( vertex, Vector2D(0, 0)));
		}
	}

	void InsertVertex(int index, const GraphItem<T>& vertex) {
		if (index >= 0 && index <= static_cast<int>(_vertices.size())) {
			_vertices.insert(_vertices.begin() + index, vertex);
		}
	}

	constexpr size_t GetVertexCount() const {
		return _vertices.size();
	}

	Graph<T> BuildPlanarGraph() const {
		const size_t size = _vertices.size();
		if (size < 4) {
			return *this;
		}
		Graph<T> planarGraph;
		planarGraph.AddVertex(_vertices[0]);
		for (size_t i = 0; i < size - 1; ++i) {
			const GraphItem<T>& start = _vertices[i];
			const GraphItem<T>& end = _vertices[i + 1];
			const Vector2D startPos = start.GetPosition();
			const Vector2D endPos = end.GetPosition();
			LineSegment currentSeg(startPos, endPos);
			std::vector<Vector2D> intersections;
			for (size_t j = 0; j < size - 1; ++j) {
				if (i == j || j+1 == i || i+1 == j) {
					continue;
				}
				LineSegment otherSeg(_vertices[j].GetPosition(), _vertices[j + 1].GetPosition());
				if (not currentSeg.IsCrossing(otherSeg)) continue;
				
				Vector2D crossingPoint = currentSeg.GetCrossingPoint(otherSeg);
				bool duplicated = false;
				for (const auto& point : intersections) {
					const double dx = point.x - crossingPoint.x;
					const double dy = point.y - crossingPoint.y;
					const double distanceSquared = dx * dx + dy * dy;
					if (distanceSquared < 0.000001) {
						duplicated = true;
						break;
					}
				}
				if (not duplicated) {
					intersections.push_back(crossingPoint);
				}
			}
			std::sort(intersections.begin(), intersections.end(),
				[&](const Vector2D& lhs,
					const Vector2D& rhs) {
						const double ldx = lhs.x - startPos.x;
						const double ldy = lhs.y - startPos.y;
						const double rdx = rhs.x - startPos.x;
						const double rdy = rhs.y - startPos.y;
						const double lhsDistance = ldx * ldx + ldy * ldy;
						const double rhsDistance = rdx * rdx + rdy * rdy;
						return lhsDistance < rhsDistance;
				}
			);
			for (const Vector2D& point : intersections) {
				GraphItem<T> crossingItem(T{}, point);
				planarGraph.AddVertex(crossingItem);
			}

			planarGraph.AddVertex(end);
		}

		return planarGraph;
	}

	std::vector<std::vector<GraphItem<T>>> GetCycles() const {
		std::vector<std::vector<GraphItem<T>>> cycles;
		std::vector<GraphItem<T>> vertices = _vertices, path;
		for (const auto& v : vertices) {
			auto it = std::find(path.rbegin(), path.rend(), v);
			if (it != path.rend()) {
				std::vector<GraphItem<T>> cycle;
				for (auto cycleIt = std::prev(it.base()); cycleIt != path.end(); ++cycleIt) {
					cycle.push_back(*cycleIt);
				}
				cycles.push_back(cycle);
			}
			path.push_back(v);
		}
		return cycles;
	}

	GraphItem<T> operator[](size_t index) const {
		if (index >= _vertices.size()) {
			throw std::out_of_range("Index out of range");
		}
		return _vertices[index];
	}
};