#include "EffekseerEmitter.h"
#include <EffekseerForDXLib.h>

void EffekseerEmitter2D::_Update(float dt) {
	if (_effect) {
		SetPosPlayingEffekseer2DEffect(_effectHandle, GetDrawPosition().x, GetDrawPosition().y, 0);
		SetScalePlayingEffekseer2DEffect(_effectHandle, GetDrawScale().x, GetDrawScale().y, 1.0f);
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

void EffekseerEmitter2D::SendTrigger(int index) {
	if (_effect && _effectHandle >= 0) {
		GetEffekseer2DManager()->SendTrigger(_effectHandle, index);
	}
}