#include "Scene.h"
#include <SceneManager.h>
#include <Animation/Tween.h>
#include <Timer.h>
#include <Camera.h>
#include <Graphic/2D.h>
#include <Lua/LuaSceneScript.h>
#include <Scene/Button/Button.h>
#include <Scene/Texture/AnimatedSprite2D.h>
#include <Backend/Backend.h>

Scene::~Scene() {
	delete _luaScript;
}

bool Scene::IsShouldDraw() const {
	if (not visible || not _isEnabled) return false;
	if (parent.expired()) {
		return true;
	}
	else {
		return parent.lock()->IsShouldDraw();
	}
}

void Scene::Clear() {
	KillAllTimers();
	KillAllTweenGroups();
	_scenes.clear();
}

void Scene::Load() {
	if (_isLoaded) {
		if (_luaScript and not _luaScript->IsLoaded()) {
			_luaScript->Load();
		}
		return;
	}
	ID = SceneManager.GetID();
	_Load();
	auto scenesCopy = _scenes;
	for (const auto& s : scenesCopy) {
		s->SetParent(weak_from_this());
		s->Load();
	}
	_isLoaded = true;
	if (_luaScript) {
		_luaScript->Load();
	}
}

void Scene::Release() {
	_Release();
	if (_luaScript) {
		_luaScript->Release();
	}
	
	const auto scenesCopy = _scenes;
	for (const auto& s : scenesCopy) {
		s->Release();
	}
}

void Scene::Update(float dt) {
	if (SceneManager.IsCancelUpdate() || not _isEnabled) {
		return;
	}
	_Update(dt);
	for (const auto& cb : _constantBuffer) {
		CoreBackend::Shader::UpdateConstantBuffer(cb.handle);
	}
	if (_luaScript) {
		_luaScript->Update(dt);
	}
	if (SceneManager.IsCancelUpdate()) {
		return;
	}
	const auto scenesCopy = _scenes;
	for (auto it=scenesCopy.rbegin(); it!=scenesCopy.rend(); ++it) {
		(*it)->Update(dt);
	}

	if (IsShouldDraw()) {
		if (IsUIMode()) {
			SceneManager.AddDrawUITarget(shared_from_this());
		}
		else {
			SceneManager.AddDrawTarget(shared_from_this());
		}
	}
}

bool Scene::AttachLuaScript(const fs::path& path) {
	delete _luaScript;
	_luaScript = new LuaSceneScript(this, path.string());
	if (_isLoaded) {
		return _luaScript->Load();
	}
	return true;
}

bool Scene::IsLuaScriptLoaded() const {
	return _luaScript and _luaScript->IsLoaded();
}

void Scene::BindLuaButtonEvents(Scene* target, const std::vector<std::pair<std::string, std::string>>& events) {
	if (_luaScript) {
		_luaScript->BindButton(dynamic_cast<Button*>(target), events);
	}
}

void Scene::BindLuaAnimatedSpriteEvents(Scene* target, const std::vector<std::pair<std::string, std::string>>& events) {
	if (_luaScript) {
		_luaScript->BindAnimatedSprite(dynamic_cast<AnimatedSprite2D*>(target), events);
	}
}

void Scene::Draw() const {
	if (not IsShouldDraw()) return;
	CoreBackend::Drawing::SetAlphaBlend(GetAlpha());
	CoreBackend::Drawing::SetBrightness(static_cast<int>(modulate.x), static_cast<int>(modulate.y), static_cast<int>(modulate.z));
	if (_pixelShader && not _constantBuffer.empty()) {
		for (const auto& cb : _constantBuffer) {
			CoreBackend::Shader::SetConstantBuffer(cb.handle, cb.slot);
		}
	}
	_Draw();
#if defined(_DEBUG)
	if (PropertyManager.IsDebugMode()) {
		NovotRenderer::DrawPoint(GetDrawYSortPoint(), CoreBackend::Drawing::Color(255, 0, 0), 10);
	}
#endif
	CoreBackend::Drawing::SetNoBlend();
}

SharedPtr<Scene> Scene::GetChild(int index) const {
	if (index < 0 || index >= static_cast<int>(_scenes.size())) {
		return nullptr;
	}
	return _scenes[index];
}

Vector2D Scene::GetParentGlobalPosition() const {
	if (parent.expired()) {
		return Vec2D_0;
	}
	else {
		return parent.lock()->GetGlobalPosition();
	}
}
Vector2D Scene::GetParentScale() const {
	if (parent.expired()) {
		return Vec2D_1;
	}
	else {
		return parent.lock()->GetScale();
	}
}

Vector2D Scene::GetGlobalPosition() const {
	const Vector2D scaledPosition = (position + offset) * GetParentScale();
	if (auto ptr = parent.lock()) {
		const double parentRotation = ptr->GetRotation();
		const double cosine = std::cos(parentRotation);
		const double sine = std::sin(parentRotation);
		return ptr->GetGlobalPosition() + Vector2D(
			scaledPosition.x * cosine - scaledPosition.y * sine,
			scaledPosition.x * sine + scaledPosition.y * cosine
		);
	}
	return scaledPosition;
}

Vector2D Scene::GetDrawPosition() const {
	if (IsIgnoringCamera()) {
		return GetGlobalPosition();
	}
	return Camera.WorldToScreen(GetGlobalPosition());
}

Vector2D Scene::GetDrawPoint(const Vector2D& localPosition) const {
	const Vector2D scaledPosition = localPosition * GetDrawScale();
	const double angle = GetRotation();
	const double cosine = std::cos(angle);
	const double sine = std::sin(angle);
	return GetDrawPosition() + Vector2D(
		scaledPosition.x * cosine - scaledPosition.y * sine,
		scaledPosition.x * sine + scaledPosition.y * cosine
	);
}

Vector2D Scene::GetDrawScale() const {
	if (IsIgnoringCamera()) {
		return GetScale();
	}
	return GetScale() * Camera.GetZoom();
}

Scene* Scene::SetGlobalPosition(const Vector2D& pos) {
	if (parent.expired()) {
		position = pos - offset;
	}
	else {
		const auto p = parent.lock();
		const Vector2D pScale = p->GetScale();
		const Vector2D delta = pos - p->GetGlobalPosition();
		position = Vector2D(
			pScale.x != 0 ? delta.x / pScale.x : 0,
			pScale.y != 0 ? delta.y / pScale.y : 0
		) - offset;
	}
	return this;
}

Vector2D Scene::GetScale() const {
	if (parent.expired() || _ignoreParentScale) {
		return scale;
	}
	else {
		return parent.lock()->GetScale() * scale;
	}
}

Vector3D Scene::GetModulate() const {
	if (parent.expired()) {
		return modulate;
	}
	else {
		const auto p = parent.lock();
		const Vector3D pModulate = p->GetModulate();
		return Vector3D(
			modulate.x * pModulate.x / 255.0,
			modulate.y * pModulate.y / 255.0,
			modulate.z * pModulate.z / 255.0
		);
	}
}

const Vector2D& Scene::GetThisSize() const {
	return GetRawSize() * GetScale();
}

Vector2D Scene::GetSize() const {
	Vector2D result = GetThisSize();
	for (const auto scene : _scenes) {	
		const auto childSize = scene->GetSize();
		if (result.x <  childSize.x) {
			result.x = childSize.x;
		}
		if (result.y < childSize.y) {
			result.y = childSize.y;
		}
	}
	return result;
}

std::shared_ptr<Timer> Scene::CreateTimer(double time, const std::function<void()> callback, int loop) {
	auto timer = CreateScene<Timer>();
	timer->SetTime(time)
		->SetLoop(loop)
		->OnTimeout(callback)
		->OnCompleted([timer]() {
			timer->Free();
		});
	_timers.push_back(timer);
	return timer;
}

std::shared_ptr<TweenGroup> Scene::CreateTweenGroup() {
	auto tweenGroup = CreateScene<TweenGroup>();
	auto ptr = tweenGroup->weak_from_this();
	tweenGroup->OnComplete([ptr]() {
		if (auto tg = ptr.lock()) {
			tg->Free();
		}
	});
	_tweenGroups.push_back(tweenGroup);
	return tweenGroup;
}
void Scene::KillAllTweenGroups() {
	for (auto& tweenGroup : _tweenGroups) {
		if (auto tg = tweenGroup.lock()) {
			tg->Kill();
		}
	}
	_tweenGroups.clear();
}

void Scene::SkipAllTweenGroups() {
	for (auto& tweenGroup : _tweenGroups) {
		if (auto tg = tweenGroup.lock()) {
			tg->Skip();
		}
	}
	_tweenGroups.clear();
}

void Scene::KillAllTimers() {
	for (auto& timer : _timers) {
		if (auto t = timer.lock()) {
			t->Kill();
		}
	}
	_timers.clear();
}
