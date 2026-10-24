#include "LuaSceneScript.h"
#include <Scene.h>
#include <Scene\Button\Button.h>
#include <Scene\Texture\AnimatedSprite2D.h>
#include <Mouse\MouseEvent.h>
#include <lua.hpp>
#include <iostream>
#include <cstring>

namespace {
	constexpr const char* SceneMetatable = "Novot.Scene";

	Scene* GetScene(lua_State* state, int index) {
		auto value = static_cast<Scene**>(luaL_checkudata(state, index, SceneMetatable));
		return *value;
	}

	int SceneIndex(lua_State* state) {
		auto scene = GetScene(state, 1);
		const char* key = luaL_checkstring(state, 2);
		if (std::strcmp(key, "name") == 0) {
			lua_pushstring(state, scene->GetName().c_str());
		}
		else if (std::strcmp(key, "alpha") == 0) {
			lua_pushinteger(state, scene->alpha);
		}
		else if (std::strcmp(key, "visible") == 0) {
			lua_pushboolean(state, scene->IsVisible());
		}
		else if (std::strcmp(key, "enabled") == 0) {
			lua_pushboolean(state, scene->IsEnabled());
		}
		else if (std::strcmp(key, "position") == 0 || std::strcmp(key, "offset") == 0 || std::strcmp(key, "scale") == 0) {
			Vector2D value;
			if (std::strcmp(key, "position") == 0) value = scene->GetPosition();
			else if (std::strcmp(key, "offset") == 0) value = scene->GetOffset();
			else value = scene->scale;
			lua_createtable(state, 0, 2);
			lua_pushnumber(state, value.x);
			lua_setfield(state, -2, "x");
			lua_pushnumber(state, value.y);
			lua_setfield(state, -2, "y");
		}
		else {
			lua_pushnil(state);
		}
		return 1;
	}

	Vector2D ReadVector(lua_State* state, int index) {
		luaL_checktype(state, index, LUA_TTABLE);
		lua_getfield(state, index, "x");
		double x = luaL_checknumber(state, -1);
		lua_pop(state, 1);
		lua_getfield(state, index, "y");
		double y = luaL_checknumber(state, -1);
		lua_pop(state, 1);
		return Vector2D(x, y);
	}

	int SceneNewIndex(lua_State* state) {
		auto scene = GetScene(state, 1);
		const char* key = luaL_checkstring(state, 2);
		if (std::strcmp(key, "name") == 0) scene->SetName(luaL_checkstring(state, 3));
		else if (std::strcmp(key, "alpha") == 0) scene->SetAlpha(static_cast<int>(luaL_checkinteger(state, 3)));
		else if (std::strcmp(key, "visible") == 0) scene->SetVisible(not (lua_toboolean(state, 3) == 0));
		else if (std::strcmp(key, "position") == 0) scene->SetPosition(ReadVector(state, 3));
		else if (std::strcmp(key, "offset") == 0) scene->SetOffset(ReadVector(state, 3));
		else if (std::strcmp(key, "scale") == 0) scene->SetScale(ReadVector(state, 3));
		else if (std::strcmp(key, "enabled") == 0) {
			if (not (lua_toboolean(state, 3) == 0)) scene->Enable();
			else scene->Disable();
		}
		return 0;
	}

	void PushEvent(lua_State* state, const MouseEvent& event) {
		lua_createtable(state, 0, 5);
		lua_createtable(state, 0, 2);
		lua_pushnumber(state, event.position.x);
		lua_setfield(state, -2, "x");
		lua_pushnumber(state, event.position.y);
		lua_setfield(state, -2, "y");
		lua_setfield(state, -2, "position");
		lua_createtable(state, 0, 2);
		lua_pushnumber(state, event.globalPosition.x);
		lua_setfield(state, -2, "x");
		lua_pushnumber(state, event.globalPosition.y);
		lua_setfield(state, -2, "y");
		lua_setfield(state, -2, "global_position");
		lua_pushboolean(state, event.just);
		lua_setfield(state, -2, "just");
	}
}

LuaSceneScript::LuaSceneScript(Scene* owner, const std::string& filePath) : _owner(owner), _filePath(filePath) {}

LuaSceneScript::~LuaSceneScript() {
	Release();
}

void LuaSceneScript::PushScene(Scene* scene) {
	auto value = static_cast<Scene**>(lua_newuserdatauv(_state, sizeof(Scene*), 0));
	*value = scene;
	luaL_getmetatable(_state, SceneMetatable);
	lua_setmetatable(_state, -2);
}

void LuaSceneScript::PushMouseEvent(const MouseEvent& event) {
	PushEvent(_state, event);
}

bool LuaSceneScript::Load() {
	Release();
	_state = luaL_newstate();
	if (not _state) return false;
	luaL_openlibs(_state);
	luaL_newmetatable(_state, SceneMetatable);
	lua_pushcfunction(_state, SceneIndex);
	lua_setfield(_state, -2, "__index");
	lua_pushcfunction(_state, SceneNewIndex);
	lua_setfield(_state, -2, "__newindex");
	lua_pop(_state, 1);
	if (not (luaL_loadfile(_state, _filePath.c_str()) == LUA_OK) || not (lua_pcall(_state, 0, 0, 0) == LUA_OK)) {
		std::cerr << "Lua load error (" << _filePath << "): " << lua_tostring(_state, -1) << std::endl;
		Release();
		return false;
	}
	PushScene(_owner);
	if (not Call("on_load", 1)) {
		Release();
		return false;
	}
	if (auto button = dynamic_cast<Button*>(_owner)) {
		BindButton(button, { {"left_pressed", "on_left_pressed"}, {"left_released", "on_left_released"} });
	}
	if (auto sprite = dynamic_cast<AnimatedSprite2D*>(_owner)) {
		BindAnimatedSprite(sprite, { {"animation_changed", "on_animation_changed"}, {"animation_finished", "on_animation_finished"} });
	}
	return true;
}

bool LuaSceneScript::Call(const char* functionName, int argumentCount) {
	lua_getglobal(_state, functionName);
	if (not lua_isfunction(_state, -1)) {
		lua_pop(_state, argumentCount + 1);
		return true;
	}
	lua_insert(_state, -argumentCount - 1);
	if (not (lua_pcall(_state, argumentCount, 0, 0) == LUA_OK)) {
		std::cerr << "Lua callback error (" << _filePath << ", " << functionName << "): " << lua_tostring(_state, -1) << std::endl;
		lua_pop(_state, 1);
		return false;
	}
	return true;
}

void LuaSceneScript::Update(float dt) {
	if (not _state) return;
	PushScene(_owner);
	lua_pushnumber(_state, dt);
	Call("on_update", 2);
}

void LuaSceneScript::Release() {
	for (auto& disconnect : _disconnectors) disconnect();
	_disconnectors.clear();
	if (_state) {
		PushScene(_owner);
		Call("on_release", 1);
		lua_close(_state);
		_state = nullptr;
	}
}

void LuaSceneScript::BindButton(Button* button, const std::vector<std::pair<std::string, std::string>>& events) {
	if (not button || not _state) return;
	for (const auto& [eventName, functionName] : events) {
		if (eventName == "left_pressed") {
			const int id = button->LeftPressed.Connect([this, button, functionName](const MouseEvent& event) {
				PushScene(button);
				PushMouseEvent(event);
				Call(functionName.c_str(), 2);
			});
			_disconnectors.emplace_back([button, id]() { button->LeftPressed.Disconnect(id); });
		}
		else if (eventName == "left_released") {
			const int id = button->LeftReleased.Connect([this, button, functionName](const MouseEvent& event) {
				PushScene(button);
				PushMouseEvent(event);
				Call(functionName.c_str(), 2);
			});
			_disconnectors.emplace_back([button, id]() { button->LeftReleased.Disconnect(id); });
		}
	}
}

void LuaSceneScript::BindAnimatedSprite(AnimatedSprite2D* sprite, const std::vector<std::pair<std::string, std::string>>& events) {
	if (not sprite || not _state) return;
	for (const auto& [eventName, functionName] : events) {
		if (eventName == "animation_changed") {
			const int id = sprite->AnimationChanged.Connect([this, sprite, functionName](int animation) {
				PushScene(sprite);
				lua_pushinteger(_state, animation);
				Call(functionName.c_str(), 2);
			});
			_disconnectors.emplace_back([sprite, id]() { sprite->AnimationChanged.Disconnect(id); });
		}
		else if (eventName == "animation_finished") {
			const int id = sprite->AnimationFinished.Connect([this, sprite, functionName](int animation) {
				PushScene(sprite);
				lua_pushinteger(_state, animation);
				Call(functionName.c_str(), 2);
			});
			_disconnectors.emplace_back([sprite, id]() { sprite->AnimationFinished.Disconnect(id); });
		}
	}
}
