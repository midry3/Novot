#pragma once
#include "GridContainer.h"
#include <Scene/Shape/ColorPolygon.h>
#include <Backend/Backend.h>

enum class ProjectionType {
	Orthographic, // Standard 2D orthogonal grid
	Isometric,    // Isometric 2.5D projection
	Perspective,  // Perspective projection towards vanishing point
	Oblique,      // Oblique (Cabinet/Cavalier) projection
	Custom        // Custom transformation using axis vectors
};

class ProjectionGridContainer : public GridContainer {
protected:
	ProjectionType _projectionType = ProjectionType::Isometric;

	// Axis vectors for Isometric / Custom projection
	Vector2D _axisX = Vector2D(1.0, 0.5);
	Vector2D _axisY = Vector2D(-1.0, 0.5);

	// Perspective projection parameters
	float _cameraDistance = 500.0f;
	Vector2D _vanishingPoint = Vector2D(0.0, 0.0);
	bool _applyPerspectiveScale = true;

	// Oblique projection parameter
	Vector2D _obliqueOffset = Vector2D(0.5, -0.5);

	// Grid base polygons
	bool _showGridBase = true;
	unsigned int _defaultCellColor = CoreBackend::Drawing::Color(100, 140, 200);
	std::vector<unsigned int> _cellColors;
	std::vector<std::shared_ptr<ColorPolygon>> _gridPolygons;

	void _Update(float dt) override;
	void _SyncGridPolygons();

public:
	ProjectionGridContainer() = default;
	~ProjectionGridContainer() = default;

	ProjectionGridContainer* SetProjectionType(ProjectionType type) {
		_projectionType = type;
		return this;
	}

	ProjectionType GetProjectionType() const {
		return _projectionType;
	}

	ProjectionGridContainer* SetAxes(const Vector2D& axisX, const Vector2D& axisY) {
		_axisX = axisX;
		_axisY = axisY;
		return this;
	}

	ProjectionGridContainer* SetPerspectiveParams(float cameraDistance, const Vector2D& vanishingPoint = Vector2D(0.0, 0.0), bool applyScale = true) {
		_cameraDistance = cameraDistance;
		_vanishingPoint = vanishingPoint;
		_applyPerspectiveScale = applyScale;
		return this;
	}

	ProjectionGridContainer* SetObliqueOffset(const Vector2D& offset) {
		_obliqueOffset = offset;
		return this;
	}

	ProjectionGridContainer* SetShowGridBase(bool enable) {
		_showGridBase = enable;
		return this;
	}

	bool IsGridBaseShown() const {
		return _showGridBase;
	}

	ProjectionGridContainer* SetDefaultCellColor(unsigned int color) {
		_defaultCellColor = color;
		return this;
	}

	ProjectionGridContainer* SetCellColor(size_t gx, size_t gy, unsigned int color);

	std::shared_ptr<ColorPolygon> GetCellPolygon(size_t gx, size_t gy) const;

	// Calculates relative screen position from continuous grid coordinates (gx, gy)
	Vector2D GridToScreenPosition(double gx, double gy) const;

	Vector2D GridToScreenPosition(size_t gx, size_t gy) const {
		return GridToScreenPosition(static_cast<double>(gx), static_cast<double>(gy));
	}

	// Gets the 4 vertices of a cell's base polygon in screen space
	std::vector<Vector2D> GetCellVertices(double gx, double gy) const;

	// Converts relative screen position back to estimated grid coordinate (gx, gy)
	Vector2D ScreenToGridPosition(const Vector2D& screenPos) const;
};