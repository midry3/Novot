#include "2D.h"
#include <DxLib.h>

namespace NovotRenderer {
	void DrawLine(const Vector2D& p1, const Vector2D& p2, unsigned int color, int thickness) {
		DxLib::DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, color, thickness);
	}

	void DrawPolygon(const std::vector<Vector2D>& points, unsigned int color, bool fill) {
		if (points.size() < 3) return;
		if (fill) {
			if (points.size() == 3) {
				DxLib::DrawTriangle(
					static_cast<int>(points[0].x), static_cast<int>(points[0].y),
					static_cast<int>(points[1].x), static_cast<int>(points[1].y),
					static_cast<int>(points[2].x), static_cast<int>(points[2].y),
					color, TRUE
				);
			}
			else if (points.size() == 4) {
				DxLib::DrawQuadrangle(
					static_cast<int>(points[0].x), static_cast<int>(points[0].y),
					static_cast<int>(points[1].x), static_cast<int>(points[1].y),
					static_cast<int>(points[2].x), static_cast<int>(points[2].y),
					static_cast<int>(points[3].x), static_cast<int>(points[3].y),
					color, TRUE
				);
			}
			else {
				for (size_t i = 1; i + 1 < points.size(); ++i) {
					DxLib::DrawTriangle(
						static_cast<int>(points[0].x), static_cast<int>(points[0].y),
						static_cast<int>(points[i].x), static_cast<int>(points[i].y),
						static_cast<int>(points[i + 1].x), static_cast<int>(points[i + 1].y),
						color, TRUE
					);
				}
			}
		}
		else {
			auto it = points.begin();
			auto prev = it;
			it++;
			for (; it != points.end(); it++, prev++) {
				DxLib::DrawLine(static_cast<int>(prev->x), static_cast<int>(prev->y), static_cast<int>(it->x), static_cast<int>(it->y), color);
			}
			DxLib::DrawLine(static_cast<int>(points.back().x), static_cast<int>(points.back().y), static_cast<int>(points.front().x), static_cast<int>(points.front().y), color);
		}
	}

	void DrawRect(const Vector2D& topLeft, const Vector2D& bottomRight, unsigned int color, bool filled) {
		DrawPolygon({ topLeft, Vector2D(bottomRight.x, topLeft.y), bottomRight, Vector2D(topLeft.x, bottomRight.y) }, color, filled);
	}
}