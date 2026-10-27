#pragma once
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <lua.hpp>

class Scene;

class LuaSceneLoader {
public:
	using TypeFactory = std::function<std::shared_ptr<Scene>()>;
	using TypeConfigurator = std::function<bool(const std::shared_ptr<Scene>&, lua_State*, int, const std::filesystem::path&, std::string*)>;

	// Register a game-specific Scene type before loading Lua scene files.
	// The configurator receives the node table's stack index and the source file path.
	static bool RegisterType(const std::string& type, TypeFactory factory, TypeConfigurator configurator = {});
	static bool UnregisterType(const std::string& type);
	static std::shared_ptr<Scene> Load(const std::filesystem::path& path, std::string* errorMessage = nullptr);
};
