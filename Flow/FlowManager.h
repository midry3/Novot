#pragma once

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <Scene.h>
#include <nlohmann/json.hpp>
#include <Singleton.h>


class _FlowManager : public Singleton<_FlowManager> {
public:
	using SceneFactory = std::function<std::shared_ptr<Scene>()>;

	void Load(const fs::path& file);
	void RegisterScene(const std::string& name, SceneFactory factory);
	void RegisterScenes(std::initializer_list<std::pair<std::string, SceneFactory>> scenes);
	void Start();
	void GoTo(const std::string& nodeId);

	const std::string& GetCurrentNodeId() const { return _currentNodeId; }

private:
	nlohmann::json _flow;
	std::unordered_map<std::string, SceneFactory> _sceneFactories;
	std::string _currentNodeId;

	const nlohmann::json& GetNode(const std::string& nodeId) const;
	void StartEiga(const nlohmann::json& node);
	void StartScene(const nlohmann::json& node);
};

#define FlowManager _FlowManager::GetInstance()