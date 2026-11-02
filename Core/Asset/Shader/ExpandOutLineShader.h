#pragma once
#include "PixelShader.h"
#include <Backend/Backend.h>

struct alignas(16) ExpandOutLineShaderParam
{
	float glowColor[4];   // RGBA
	float outlineWidth;   // 輪郭の太さ
	float time;           // 時間
	float expandSpeed;	  // 輪郭の拡大速度
	float loopInterval;	  // ループ間隔
	float textureSize[2]; // 幅, 高さ
	float padding[2];     // パディング
};

struct ExpandOutLineShaderBuffer {
	int handle;
	ExpandOutLineShaderParam* param;
};

class ExpandOutLineShader : public PixelShader {
protected:
	int Load() override {
		file = "ExpandOutLineShader.cso";
		return PixelShader::Load();
	}
public:
	static const ExpandOutLineShaderBuffer CreateConstantBuffer() {
		ExpandOutLineShaderBuffer buffer;
		buffer.handle = CoreBackend::Shader::CreateBuffer(sizeof(ExpandOutLineShaderParam));
		buffer.param = static_cast<ExpandOutLineShaderParam*>(CoreBackend::Shader::Buffer(buffer.handle));
		return buffer;
	}
};