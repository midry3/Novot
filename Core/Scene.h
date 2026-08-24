#pragma once
#include <NovotCore.h>
#include "MathUtils.h"
#include <memory>
#include <vector>

class Scene : public std::enable_shared_from_this<Scene> {
protected:
	bool _isLoaded = false;
	std::weak_ptr<Scene> parent;
	std::vector<std::shared_ptr<Scene>> scenes;
	int ID = 0;
	bool visible = true;
	virtual void _Load() {};
	virtual void _Release() {};
	virtual void _Update(float dt) {};
	virtual void _Draw() const {};
	template <std::derived_from<Asset> T>
	std::shared_ptr<T> LoadAsset(const std::string& path) {
		return AssetLoader.LoadAsset<T>(path, ID);
	}
public:
	int alpha = 255;
	Vector2D position;
	Vector3D modulate = Vector3D(255, 255, 255);
	Scene() : position(0, 0) {}
	virtual ~Scene() = default;
	void Draw() const;

	bool IsShouldDraw() const;
	void Load();
	void Update(float dt);
	void Release();
	Vector2D GetPosition() const {
		return position;
	}
	Vector2D GetGlobalPosition() const;
	Vector2D GetDrawPosition() const;
	void SetPosition(const Vector2D& pos) {
		position = pos;
	}
	void Move(const Vector2D& delta) {
		position += delta;
	}
	void SetParent(std::weak_ptr<Scene> parentScene) {
		parent = parentScene;
	}
	void SetModulate(const Vector3D& mod) {
		modulate = mod;
	}
	Vector3D GetModulate() const {
		return modulate;
	}
	void AddScene(std::shared_ptr<Scene> scene) {
		scene->Load();
		scenes.emplace_back(scene);
		scene->SetParent(weak_from_this());
	}
	void DeleteScene(int index) {
		scenes.erase(scenes.begin() + index);
	}
	void DeleteScene(Scene* scene) {
		std::erase_if(scenes, [&scene](const std::shared_ptr<Scene>& s) { return s.get() == scene; });
	}
	void Free() {
		Release();
		if (parent.expired()) {
			scenes.clear();
		}
		else {
			parent.lock()->DeleteScene(this);
		}
	}
	void Show() {
		visible = true;
	}
	void Hide() {
		visible = false;
	}

	// Accessors for debugger
	const std::vector<std::shared_ptr<Scene>>& GetChildScenes() const {
		return scenes;
	}
	bool IsVisible() const {
		return visible;
	}
	void SetVisible(bool v) {
		visible = v;
	}

	template <std::derived_from<Scene> T>
	std::shared_ptr<T> CreateScene() {
		auto scene = std::make_shared<T>();
		AddScene(scene);
		return scene;
	}
};