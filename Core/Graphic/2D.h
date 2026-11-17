#pragma once
#include <Math/Vector2D.h>

namespace NovotRenderer {
	void DrawLine(const Vector2D& p1, const Vector2D& p2, unsigned int color, int thickness);
	void DrawLines(const std::vector<Vector2D>& points, unsigned int color, int thickness);
	void DrawPolygon(const std::vector<Vector2D>& vertices, unsigned int color, bool filled = false);
	void DrawGraphShader(
		int x, int y,
		int graphHandle,
		int transFlag,
		int pixelShaderHandle,
		float uMin = 0.0f, float vMin = 0.0f,
		float uMax = 1.0f, float vMax = 1.0f,
		float alpha = 1.0f);
	void DrawRotaGraph3Shader(
		int x, int y,
		int cx, int cy,
		double scaleX, double scaleY,
		double angle,
		int graphHandle,
		int transFlag,
		int pixelShaderHandle,
		float uMin = 0.0f, float vMin = 0.0f,
		float uMax = 1.0f, float vMax = 1.0f,
		float alpha = 1.0f);
	void DrawRect(const Vector2D& topLeft, const Vector2D& bottomRight, unsigned int color, bool filled = false);
	void DrawPoint(const Vector2D& point, unsigned int color, int size = 1);
};