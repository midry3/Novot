#include "2D.h"
#include <DxLib.h>

namespace NovotRenderer {
	void DrawLine(const Vector2D& p1, const Vector2D& p2, unsigned int color, int thickness) {
		DxLib::DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, color, thickness);
	}

	void DrawLines(const std::vector<Vector2D>& points, unsigned int color, int thickness) {
		if (points.size() < 2) return;
		for (size_t i = 0; i + 1 < points.size(); ++i) {
			DrawLine(points[i], points[i + 1], color, thickness);
		}
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

	void DrawPoint(const Vector2D& point, unsigned int color, int size) {
		DxLib::DrawBox(static_cast<int>(point.x - size / 2), static_cast<int>(point.y - size / 2), static_cast<int>(point.x + size / 2), static_cast<int>(point.y + size / 2), color, TRUE);
	}

	void DrawGraphShader(
		int x, int y,
		int graphHandle,
		int transFlag,
		int pixelShaderHandle,
		float uMin, float vMin,
		float uMax, float vMax)
	{
		int width, height;
		if (GetGraphSize(graphHandle, &width, &height) == -1)
		{
			return;
		}
		VERTEX2DSHADER vertices[6]{};
		// 左上
		vertices[0].pos = VGet(static_cast<float>(x), static_cast<float>(y), 0.0f);
		vertices[0].u = uMin;
		vertices[0].v = vMin;
		// 右上
		vertices[1].pos = VGet(static_cast<float>(x + width), static_cast<float>(y), 0.0f);
		vertices[1].u = uMax;
		vertices[1].v = vMin;
		// 右下
		vertices[2].pos = VGet(static_cast<float>(x + width), static_cast<float>(y + height), 0.0f);
		vertices[2].u = uMax;
		vertices[2].v = vMax;
		// 三角形1枚目のコピー
		vertices[3] = vertices[0];
		vertices[4] = vertices[2];
		// 左下
		vertices[5].pos = VGet(static_cast<float>(x), static_cast<float>(y + height), 0.0f);
		vertices[5].u = uMin;
		vertices[5].v = vMax;
		const COLOR_U8 whiteColor = GetColorU8(255, 255, 255, 255);
		const COLOR_U8 specColor = GetColorU8(0, 0, 0, 0);
		for (auto& v : vertices)
		{
			v.rhw = 1.0f;
			v.dif = whiteColor;
			v.spc = specColor;
			v.su = v.u;
			v.sv = v.v;
		}
		if (not transFlag)
		{
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		SetUseTextureToShader(0, graphHandle);
		SetUsePixelShader(pixelShaderHandle);
		DrawPrimitive2DToShader(
			vertices,
			6,
			DX_PRIMTYPE_TRIANGLELIST
		);
		SetUseTextureToShader(0, -1);
		SetUsePixelShader(-1);
	}

    void DrawRotaGraph3Shader(
        int x, int y,
        int cx, int cy,
        double scaleX, double scaleY,
        double angle,
        int graphHandle,
        int transFlag,
        int pixelShaderHandle,
        float uMin, float vMin,
        float uMax, float vMax)
    {
        int width, height;
        if (GetGraphSize(graphHandle, &width, &height) == -1)
        {
            return;
        }

        const float left = static_cast<float>(-cx * scaleX);
        const float top = static_cast<float>(-cy * scaleY);
        const float right = static_cast<float>((width - cx) * scaleX);
        const float bottom = static_cast<float>((height - cy) * scaleY);

        const float cosA = static_cast<float>(cos(angle));
        const float sinA = static_cast<float>(sin(angle));

        auto Rotate = [&](float px, float py) -> VECTOR
            {
                return VGet(
                    static_cast<float>(x) + (px * cosA - py * sinA),
                    static_cast<float>(y) + (px * sinA + py * cosA),
                    0.0f
                );
            };

        VERTEX2DSHADER vertices[6]{};

        // 左上
        vertices[0].pos = Rotate(left, top);
        vertices[0].u = uMin;
        vertices[0].v = vMin;

        // 右上
        vertices[1].pos = Rotate(right, top);
        vertices[1].u = uMax;
        vertices[1].v = vMin;

        // 右下
        vertices[2].pos = Rotate(right, bottom);
        vertices[2].u = uMax;
        vertices[2].v = vMax;

        // 三角形1枚目のコピー
        vertices[3] = vertices[0];
        vertices[4] = vertices[2];

        // 左下
        vertices[5].pos = Rotate(left, bottom);
        vertices[5].u = uMin;
        vertices[5].v = vMax;

        const COLOR_U8 whiteColor = GetColorU8(255, 255, 255, 255);
        const COLOR_U8 specColor = GetColorU8(0, 0, 0, 0);

        for (auto& v : vertices)
        {
            v.rhw = 1.0f;
            v.dif = whiteColor;
            v.spc = specColor;
            v.su = v.u;
            v.sv = v.v;
        }

        if (not transFlag)
        {
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

        SetUseTextureToShader(0, graphHandle);
        SetUsePixelShader(pixelShaderHandle);

        DrawPrimitive2DToShader(
            vertices,
            6,
            DX_PRIMTYPE_TRIANGLELIST
        );

        SetUseTextureToShader(0, -1);
        SetUsePixelShader(-1);
    }
}