#pragma once
#include <NovotCore.h>
#include <Asset/Shader/PixelShader.h>
#include <Math/Vector2D.h>
#include <Math/Vector3D.h>
#include <memory>
#include <vector>

class TweenGroup;
class Timer;
class LuaSceneScript;

class Scene : public std::enable_shared_from_this<Scene> {
protected:
	struct ConstantBufferData {
		int handle = -1;
		int slot = -1;
	};

	bool _isLoaded = false;
	std::string _name;
	std::weak_ptr<Scene> parent;
	std::vector<std::shared_ptr<Scene>> _scenes;
	std::vector<std::weak_ptr<TweenGroup>> _tweenGroups;
	std::vector<std::weak_ptr<Timer>> _timers;
	std::shared_ptr<PixelShader> _pixelShader;
	LuaSceneScript* _luaScript = nullptr;
	int ID = 0, ZIdex = 0;
	int _ySortOffset = 0;
	bool visible = true;
	bool _isIgnoreParentAlpha = false;
	bool _ySortEnabled = false;
	bool _isEnabled = true;
	bool _ignoreCamera = false;
	bool _isUI = false;
	Vector<ConstantBufferData>  _constantBuffer;

	virtual void _Load() {};
	virtual void _Release() {};
	virtual void _Update(float dt) {};
	virtual void _Draw() const {};
public:
	Signal<bool> VisibilityChanged;
	int alpha = 255;
	double rotation = 0.0;
	Vector2D position, offset, scale, rotationCenter;
	Vector3D modulate = Vector3D(255, 255, 255);
	Scene() : position(0, 0), offset(0, 0), scale(1, 1) {}
	virtual ~Scene();
	void Draw() const;
	virtual void Clear();
	[[nodiscard]] bool IsShouldDraw() const;
	virtual void Load();
	virtual void Update(float dt);
	virtual void Release();
	bool AttachLuaScript(const fs::path& path);
	bool IsLuaScriptLoaded() const;
	void BindLuaButtonEvents(Scene* target, const std::vector<std::pair<std::string, std::string>>& events);
	void BindLuaAnimatedSpriteEvents(Scene* target, const std::vector<std::pair<std::string, std::string>>& events);
	void Enable() {
		_isEnabled = true;
		VisibilityChanged.Emit(true);
	}
	void Disable() {
		_isEnabled = false;
		VisibilityChanged.Emit(false);
	}
	[[nodiscard]] bool IsEnabled() const {
		return _isEnabled;
	}
	Scene* SetName(const std::string& newName) {
		_name = newName;
		return this;
	}
	[[nodiscard]] std::string GetName() const {
		if (_name.empty()) {
			return typeid(*this).name();
		}
		return _name;
	}
	[[nodiscard]] int GetID() const {
		return ID;
	}
	[[nodiscard]] int GetZIndex() const {
		return ZIdex;
	}
	[[nodiscard]] Vector2D GetPosition() const {
		return position;
	}
	[[nodiscard]] Vector2D GetParentGlobalPosition() const;

	[[nodiscard]] Vector2D GetGlobalPosition() const;
	[[nodiscard]] Vector2D GetDrawPosition() const;
	[[nodiscard]] Vector2D GetDrawPoint(const Vector2D& localPosition) const;
	[[nodiscard]] Vector2D GetDrawScale() const;
	Scene* SetPosition(const Vector2D& pos) {
		position = pos;
		return this;
	}
	Scene* SetGlobalPosition(const Vector2D& pos);
	Scene* SetOffset(const Vector2D& off) {
		offset = off;
		return this;
	}
	[[nodiscard]] Vector2D GetOffset() const {
		return offset;
	}
	Scene* SetScale(double s) {
		scale = Vector2D(s, s);
		return this;
	}
	Scene* SetScale(const Vector2D& s) {
		scale = s;
		return this;
	}
	[[nodiscard]] Vector2D GetScale() const;
	[[nodiscard]] Vector2D GetParentScale() const;
	Scene* SetYSortOffset(int offset) {
		_ySortOffset = offset;
		return this;
	}
	Scene* SetYSortOffsetRatio(double r) {
		_ySortOffset = static_cast<int>(GetSize().y * r);
		return this;
	}
	[[nodiscard]] double GetYSortPriority() const {
		return GetDrawPosition().y + _ySortOffset;
	}
#ifdef _DEBUG
	[[nodiscard]] Vector2D GetDrawYSortPoint() const {
		return { GetDrawPosition().x, GetYSortPriority() };
	}
#endif
	Scene* Move(const Vector2D& delta) {
		position += delta;
		return this;
	}
	Scene* SetAlpha(int a) {
		alpha = std::clamp(a, 0, 255);
		return this;
	}
	Scene* SetAlpha(float a) {
		alpha = Clamp(static_cast<int>(a * 255), 0, 255);
		return this;
	}
	[[nodiscard]] int GetAlpha() const {
		auto ptr = parent.lock();
		if (not _isIgnoreParentAlpha && ptr) {
			return std::clamp(alpha * ptr->GetAlpha() / 255, 0, 255);
		}
		return alpha;
	}
	Scene* SetRotation(double r) {
		rotation = r;
		return this;
	}
	[[nodiscard]] double GetRotation() const {
		if (auto ptr = parent.lock()) {
			return rotation + ptr->GetRotation();
		}
		return rotation;
	}
	Scene* RotateDegree(double degree) {
		rotation = degree * PI / 180.0;
		return this;
	}
	Scene* RotateTurn(double turn) {
		rotation = turn * 2 * PI;
		return this;
	}
	[[nodiscard]] const double GetRotationDegree() const {
		return GetRotation() * 180.0 / PI;
	}
	[[nodiscard]] const double GetRotationTurn() const {
		return GetRotation() / (2 * PI);
	}
	Scene* SetRotationCenter(const Vector2D& center) {
		rotationCenter = center;
		return this;
	}
	Scene* SetRotationCenterRatio(const Vector2D& ratio) {
		rotationCenter = GetSize() * ratio;
		return this;
	}
	[[nodiscard]] Vector2D GetRotationCenter() const {
		if (auto ptr = parent.lock()) {
			return rotationCenter + ptr->GetRotationCenter();
		}
		return rotationCenter;
	}
	Scene* IgnoreParentAlpha() {
		_isIgnoreParentAlpha = true;
		return this;
	}
	Scene* FollowParentAlpha() {
		_isIgnoreParentAlpha = false;
		return this;
	}
	Scene* SetParent(std::weak_ptr<Scene> parentScene) {
		parent = parentScene;
		return this;
	}
	Scene* SetModulate(const Vector3D& mod) {
		modulate = mod;
		return this;
	}
	Scene* IgnoreCamera() {
		_ignoreCamera = true;
		return this;
	}
	[[nodiscard]] bool IsIgnoringCamera() const {
		if (parent.expired()) {
			return _ignoreCamera;
		}
		else {
			return _ignoreCamera || parent.lock()->IsIgnoringCamera();
		}
	}
	[[nodiscard]] Vector3D GetModulate() const;
	virtual Scene* AddScene(std::shared_ptr<Scene> scene) {
		scene->Load();
		_scenes.push_back(scene);
		scene->SetParent(weak_from_this());
		return this;
	}
	Scene* SetPixelShader(std::shared_ptr<PixelShader> shader) {
		_pixelShader = shader;
		if (not _pixelShader) {
			ClearPixelShaderConstantBuffer();
		}
		return this;
	}
	template <std::derived_from<PixelShader> T>
	Scene* SetPixelShader() {
		auto shader = std::make_shared<T>();
		_pixelShader = shader;
		return this;
	}
	Scene* SetPixelShaderConstantBuffer(int handle, int slot=4) {
		_constantBuffer.push_back(ConstantBufferData {
			handle,
			slot
			});
		return this;
	}
	Scene* ClearPixelShaderConstantBuffer() {
		_constantBuffer.clear();
		return this;
	}
	[[nodiscard]] std::shared_ptr<PixelShader> GetParentPixelShader() const {
		if (auto ptr = parent.lock()) {
			return ptr->GetPixelShader();
		}
		return nullptr;
	}
	[[nodiscard]] std::shared_ptr<PixelShader> GetPixelShader() const {
		return _pixelShader ? _pixelShader : GetParentPixelShader();
	}
	[[nodiscard]] SharedPtr<Scene> GetChild(int index) const;
	Scene* SetUIMode(bool isUI) {
		_isUI = isUI;
		return this;
	}
	[[nodiscard]] bool IsUIMode() const {
		const auto& ptr = parent.lock();
		if (ptr) {
			return _isUI || ptr->IsUIMode();
		}
		return _isUI;
	}
	void DeleteScene(int index) {
		_scenes.erase(_scenes.begin() + index);
	}
	void DeleteScene(std::shared_ptr<Scene> scene) {
		std::erase_if(_scenes, [&scene](const std::shared_ptr<Scene>& s){ return s == scene; });
	}
	void DeleteFromParent() {
		if (auto ptr = parent.lock()) {
			ptr->DeleteScene(shared_from_this());
			parent = std::weak_ptr<Scene>();
		}
	}
	void Free() {
		Release();
		if (parent.expired()) {
			_scenes.clear();
		}
		else {
			DeleteFromParent();
		}
	}
	Scene* Show() {
		visible = true;
		VisibilityChanged.Emit(true);
		return this;
	}
	Scene* Hide() {
		visible = false;
		VisibilityChanged.Emit(false);
		return this;
	}
	[[nodiscard]] const std::vector<std::shared_ptr<Scene>>& GetChildScenes() const {
		return _scenes;
	}
	[[nodiscard]] int GetChildSceneCount() const {
		return static_cast<int>(_scenes.size());
	}
	[[nodiscard]] bool IsVisible() const {
		return visible;
	}
	Scene* SetVisible(bool v) {
		visible = v;
		return this;
	}
	Scene* SetYSortEnabled(bool enabled) {
		_ySortEnabled = enabled;
		return this;
	}
	[[nodiscard]] bool IsYSortEnabled() const {
		return _ySortEnabled;
	}
	[[nodiscard]] Vector2D GetSize() const;
	[[nodiscard]] virtual const Vector2D& GetRawSize() const { return Vec2D_0; }
	[[nodiscard]] const Vector2D& GetThisSize() const;
	template <std::derived_from<Scene> T>
	std::shared_ptr<T> CreateScene() {
		auto scene = std::make_shared<T>();
		AddScene(scene);
		return scene;
	}
	std::shared_ptr<TweenGroup> CreateTweenGroup();
	void KillAllTweenGroups();
	void SkipAllTweenGroups();
	std::shared_ptr<Timer> CreateTimer(double time, const std::function<void()> callback, int loop=1);
	void KillAllTimers();
	bool operator==(const Scene& other) const {
		return ID == other.GetID();
	}

	template <std::derived_from<Asset> T>
	std::shared_ptr<T> LoadAsset(const fs::path& path) {
		return AssetManager.LoadAsset<T>(path, ID);
	}

	void ReleaseAssets() {
		AssetManager.ReleaseAsset(ID);
	}
};
