#pragma once
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct lua_State;
class Scene;
class Button;
class AnimatedSprite2D;

class LuaSceneScript {
	Scene* _owner = nullptr;
	lua_State* _state = nullptr;
	std::vector<std::function<void()>> _disconnectors;
	std::string _filePath;
	bool Call(const char* functionName, int argumentCount);
	void PushScene(Scene* scene);
	void PushMouseEvent(const struct MouseEvent& event);
public:
	LuaSceneScript(Scene* owner, const std::string& filePath);
	~LuaSceneScript();
	bool Load();
	void Update(float dt);
	void Release();
	void BindButton(Button* button, const std::vector<std::pair<std::string, std::string>>& events);
	void BindAnimatedSprite(AnimatedSprite2D* sprite, const std::vector<std::pair<std::string, std::string>>& events);
	const std::string& GetFilePath() const { return _filePath; }
	bool IsLoaded() const { return not (_state == nullptr); }
};
