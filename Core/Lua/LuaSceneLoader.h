#pragma once
#include <filesystem>
#include <memory>
#include <string>

class Scene;

class LuaSceneLoader {
public:
	static std::shared_ptr<Scene> Load(const std::filesystem::path& path, std::string* errorMessage = nullptr);
};
