#include <Scene.h>
#include "LuaSceneLoader.h"
#include "LuaSceneScript.h"
#include <Object2D.h>
#include <Scene/Button/Button.h>
#include <Scene/Shape/ColorRect.h>
#include <Scene/Text/TextLabel.h>
#include <Scene/Texture/AnimatedSprite2D.h>
#include <Scene/Texture/Sprite2D.h>
#include <lua.hpp>
#include <filesystem>

namespace {
	struct PendingEvents {
		Scene* scene;
		std::vector<std::pair<std::string, std::string>> events;
	};

	struct BuildContext {
		fs::path sourcePath;
		std::vector<PendingEvents> pendingEvents;
		std::string error;
	};

	std::shared_ptr<Scene> CreateScene(const std::string& type) {
		if (type == "Scene") return std::make_shared<Scene>();
		if (type == "Object2D") return std::make_shared<Object2D>();
		if (type == "Sprite2D") return std::make_shared<Sprite2D>();
		if (type == "AnimatedSprite2D") return std::make_shared<AnimatedSprite2D>();
		if (type == "TextLabel") return std::make_shared<TextLabel>();
		if (type == "ColorRect") return std::make_shared<ColorRect>();
		if (type == "Button") return std::make_shared<Button>();
		return nullptr;
	}

	bool GetOptionalNumber(lua_State* state, int table, const char* key, double& value) {
		lua_getfield(state, table, key);
		const bool found = lua_isnumber(state, -1);
		if (found) value = lua_tonumber(state, -1);
		lua_pop(state, 1);
		return found;
	}

	bool GetOptionalString(lua_State* state, int table, const char* key, std::string& value) {
		lua_getfield(state, table, key);
		const bool found = lua_isstring(state, -1);
		if (found) value = lua_tostring(state, -1);
		lua_pop(state, 1);
		return found;
	}

	Vector2D ReadVector(lua_State* state, int table, const char* key, const Vector2D& fallback) {
		lua_getfield(state, table, key);
		if (not lua_istable(state, -1)) {
			lua_pop(state, 1);
			return fallback;
		}
		double x = fallback.x;
		double y = fallback.y;
		GetOptionalNumber(state, -1, "x", x);
		GetOptionalNumber(state, -1, "y", y);
		lua_pop(state, 1);
		return Vector2D(x, y);
	}

	std::vector<std::pair<std::string, std::string>> ReadEvents(lua_State* state, int table) {
		std::vector<std::pair<std::string, std::string>> result;
		lua_getfield(state, table, "events");
		if (lua_istable(state, -1)) {
			lua_pushnil(state);
			while (not (lua_next(state, -2) == 0)) {
				if (lua_isstring(state, -2) and lua_isstring(state, -1)) {
					result.emplace_back(lua_tostring(state, -2), lua_tostring(state, -1));
				}
				lua_pop(state, 1);
			}
		}
		lua_pop(state, 1);
		return result;
	}

	std::shared_ptr<Scene> BuildNode(lua_State* state, int table, BuildContext& context) {
		if (not lua_istable(state, table)) {
			context.error = "Each scene node must be a table.";
			return nullptr;
		}
		std::string type;
		if (not GetOptionalString(state, table, "type", type)) {
			context.error = "Each scene node needs a string 'type'.";
			return nullptr;
		}
		auto scene = CreateScene(type);
		if (not scene) {
			context.error = "Unknown scene type: " + type;
			return nullptr;
		}

		std::string name;
		if (GetOptionalString(state, table, "name", name)) scene->SetName(name);
		scene->SetPosition(ReadVector(state, table, "position", Vec2D_0));
		scene->SetOffset(ReadVector(state, table, "offset", Vec2D_0));
		scene->SetScale(ReadVector(state, table, "scale", Vec2D_1));
		double alpha = 255;
		if (GetOptionalNumber(state, table, "alpha", alpha)) scene->SetAlpha(static_cast<int>(alpha));
		lua_getfield(state, table, "visible");
		if (not lua_isnil(state, -1)) scene->SetVisible(not (lua_toboolean(state, -1) == 0));
		lua_pop(state, 1);

		if (auto sprite = std::dynamic_pointer_cast<Sprite2D>(scene)) {
			std::string texture;
			if (GetOptionalString(state, table, "texture", texture)) {
				auto texturePath = fs::path(texture);
				if (texturePath.is_relative()) texturePath = context.sourcePath.parent_path() / texturePath;
				sprite->SetTexture(texturePath);
			}
		}
		if (auto label = std::dynamic_pointer_cast<TextLabel>(scene)) {
			std::string text;
			if (GetOptionalString(state, table, "text", text)) label->SetText(text);
			double width = -1;
			if (GetOptionalNumber(state, table, "width", width)) label->SetWidth(static_cast<int>(width));
		}
		if (auto rect = std::dynamic_pointer_cast<ColorRect>(scene)) {
			rect->SetSize(ReadVector(state, table, "size", Vec2D_0));
			double color = -1;
			if (GetOptionalNumber(state, table, "color", color)) rect->SetColor(static_cast<unsigned int>(color));
		}
		if (auto button = std::dynamic_pointer_cast<Button>(scene)) {
			lua_getfield(state, table, "area");
			if (lua_istable(state, -1)) {
				std::vector<Vector2D> points;
				const auto pointCount = lua_rawlen(state, -1);
				for (lua_Integer index = 1; index <= static_cast<lua_Integer>(pointCount); ++index) {
					lua_rawgeti(state, -1, index);
					if (lua_istable(state, -1)) {
						double x = 0;
						double y = 0;
						GetOptionalNumber(state, -1, "x", x);
						GetOptionalNumber(state, -1, "y", y);
						points.emplace_back(x, y);
					}
					lua_pop(state, 1);
				}
				button->SetArea(points);
			}
			else {
				auto size = ReadVector(state, table, "size", Vec2D_0);
				if (not (size.x <= 0) && not (size.y <= 0)) {
					button->SetArea({ Vector2D(0, 0), Vector2D(size.x, 0), size, Vector2D(0, size.y) });
				}
			}
			lua_pop(state, 1);
		}

		auto events = ReadEvents(state, table);
		if (not events.empty()) context.pendingEvents.push_back({ scene.get(), std::move(events) });

		lua_getfield(state, table, "children");
		if (not lua_isnil(state, -1)) {
			if (not lua_istable(state, -1)) {
				context.error = "The 'children' value must be an array.";
				lua_pop(state, 1);
				return nullptr;
			}
			const int children = lua_gettop(state);
			const auto childCount = lua_rawlen(state, children);
			for (lua_Integer index = 1; index <= static_cast<lua_Integer>(childCount); ++index) {
				lua_rawgeti(state, children, index);
				auto child = BuildNode(state, lua_gettop(state), context);
				lua_pop(state, 1);
				if (not child) {
					lua_pop(state, 1);
					return nullptr;
				}
				scene->AddScene(child);
			}
			lua_pop(state, 1);
		}
		else {
			lua_pop(state, 1);
		}
		return scene;
	}
}

std::shared_ptr<Scene> LuaSceneLoader::Load(const std::filesystem::path& path, std::string* errorMessage) {
	if (errorMessage) errorMessage->clear();
	lua_State* state = luaL_newstate();
	if (not state) {
		if (errorMessage) *errorMessage = "Could not create Lua state.";
		return nullptr;
	}
	luaL_openlibs(state);
	const auto pathString = path.string();
	if (not (luaL_loadfile(state, pathString.c_str()) == LUA_OK) || not (lua_pcall(state, 0, 1, 0) == LUA_OK)) {
		const char* message = lua_tostring(state, -1);
		if (errorMessage) *errorMessage = message ? message : "Lua scene script failed.";
		lua_close(state);
		return nullptr;
	}
	BuildContext context{ path };
	auto root = BuildNode(state, lua_gettop(state), context);
	lua_close(state);
	if (not root) {
		if (errorMessage) *errorMessage = context.error;
		return nullptr;
	}
	if (not root->AttachLuaScript(path)) {
		if (errorMessage) *errorMessage = "Could not load Lua callbacks.";
		return nullptr;
	}
	root->Load();
	if (not root->IsLuaScriptLoaded()) {
		if (errorMessage) *errorMessage = "Lua callbacks failed to load.";
		return nullptr;
	}
	for (const auto& item : context.pendingEvents) {
		if (dynamic_cast<Button*>(item.scene)) root->BindLuaButtonEvents(item.scene, item.events);
		else if (dynamic_cast<AnimatedSprite2D*>(item.scene)) root->BindLuaAnimatedSpriteEvents(item.scene, item.events);
		else {
			if (errorMessage) *errorMessage = "Events are only supported on Button and AnimatedSprite2D nodes.";
			root->Release();
			return nullptr;
		}
	}
	return root;
}
