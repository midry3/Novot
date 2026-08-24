#include "Scene.h"
#include <SceneManager.h>

bool Scene::IsShouldDraw() const {
	if (!visible) return false;
	if (parent.expired()) {
		return true;
	}
	else {
		return parent.lock()->IsShouldDraw();
	}
}

void Scene::Load() {
	if (_isLoaded) return;
	ID = SceneManager.GetID();
	_Load();
	for (const auto& s : scenes) {
		s->Load();
	}
	_isLoaded = true;
}

void Scene::Release() {
	_Release();
	AssetLoader.ReleaseAsset(ID);
	for (const auto& s : scenes) {
		s->Release();
	}
}

void Scene::Update(float dt) {
	if (SceneManager.IsCancelUpdate()) {
		return;
	}
	_Update(dt);
	if (SceneManager.IsCancelUpdate()) {
		return;
	}
	for (const auto& s : scenes) {
		s->Update(dt);
	}
}

void Scene::Draw() const {
	if (!IsShouldDraw()) return;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	SetDrawBright(modulate.x, modulate.y, modulate.z);
	_Draw();
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	for (const auto& s : scenes) {
		s->Draw();
	}
}

Vector2D Scene::GetGlobalPosition() const {
	if (parent.expired()) {
		return position;
	}
	else {
		return parent.lock()->GetGlobalPosition() + position;
	}
}

Vector2D Scene::GetDrawPosition() const {
	return GetGlobalPosition() - Camera.GetPoint();
}