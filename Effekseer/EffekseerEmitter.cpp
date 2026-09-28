#include "EffekseerEmitter.h"
#include <EffekseerForDXLib.h>

void EffekseerEmitter2D::_Update(float dt) {
	if (_effect) {
		SetPosPlayingEffekseer2DEffect(_effectHandle, GetDrawPosition().x, GetDrawPosition().y, 0);
		SetScalePlayingEffekseer2DEffect(_effectHandle, GetScale().x, GetScale().y, 1.0f);
	}
}

void EffekseerEmitter2D::_Draw() const {
	if (_effect) {
		DrawEffekseer2D_Begin();
		DrawEffekseer2D_Draw(_effectHandle);
		DrawEffekseer2D_End();
	}
}

void EffekseerEmitter2D::Play() {
	if (_effect) {
		_effectHandle = PlayEffekseer2DEffect(_effect->Load());
	}
}

void EffekseerEmitter2D::Stop() {
	if (_effect) {
		StopEffekseer2DEffect(_effectHandle);
	}
}