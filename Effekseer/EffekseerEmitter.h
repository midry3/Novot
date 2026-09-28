#pragma once
#include "EffekseerAsset.h"
#include <Scene.h>

class EffekseerEmitter2D : public Scene {
protected:
	std::shared_ptr<EffekseerAsset> _effect;
	int _effectHandle = -1;
	void _Update(float dt) override;
	void _Draw() const override;
public:
	void Play();
	void Stop();
	EffekseerEmitter2D* SetEffect(std::string effect) {
		_effect = LoadAsset<EffekseerAsset>(effect);
		return this;
	}
};