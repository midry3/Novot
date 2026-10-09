#pragma once
#include "PixelShader.h"

struct alignas(16) OutLineShaderParam {
	float glowColor[4];   // RGBA
	float outlineWidth;   // 輪郭の太さ
	float time;		      // 時間
	float textureSize[2]; // 幅, 高さ
};

struct OutLineShaderBuffer {
	int handle;
	OutLineShaderParam* param;
};

class OutLineShader : public PixelShader {
protected:
	int Load() override {
		file = "OutLineShader.cso";
		return PixelShader::Load();
	}
public:
	static const OutLineShaderBuffer CreateConstantBuffer() {
		OutLineShaderBuffer buffer;
		buffer.handle = CreateShaderConstantBuffer(sizeof(OutLineShaderParam));
		buffer.param = static_cast<OutLineShaderParam*>(GetBufferShaderConstantBuffer(buffer.handle));
		return buffer;
	}
};