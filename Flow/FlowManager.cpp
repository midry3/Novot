#include "FlowManager.h"

#include <fstream>
#include <stdexcept>

#include <Asset/AssetManager.h>
#include <Eiga/EigaRuntime.h>
#include <SceneManager.h>

void _FlowManager::Load(const fs::path& file) {
	std::ifstream input(AssetManager.GetPath(file));
	if (not input) {
		throw std::runtime_error("Flow file could not be opened: " + file.string());
	}

	input >> _flow;
	if (not _flow.contains("start") || not _flow["start"].is_string()) {
		throw std::runtime_error("Flow file requires a string 'start' property.");
	}
	if (not _flow.contains("nodes") || not _flow["nodes"].is_object()) {
		throw std::runtime_error("Flow file requires an object 'nodes' property.");
	}
}

void _FlowManager::RegisterScene(const std::string& name, SceneFactory factory) {
	_sceneFactories.insert_or_assign(name, std::move(factory));
}

void _FlowManager::RegisterScenes(std::initializer_list<std::pair<std::string, SceneFactory>> scenes) {
	for (const auto& [name, factory] : scenes) {
		RegisterScene(name, factory);
	}
}

void _FlowManager::Start() {
	GoTo(_flow.at("start").get<std::string>());
}

void _FlowManager::GoTo(const std::string& nodeId) {
	const auto& node = GetNode(nodeId);
	_currentNodeId = nodeId;

	const std::string type = node.at("type").get<std::string>();
	if (type == "eiga") {
		StartEiga(node);
		return;
	}
	if (type == "scene") {
		StartScene(node);
		return;
	}

	throw std::runtime_error("Unknown flow node type: " + type);
}

const nlohmann::json& _FlowManager::GetNode(const std::string& nodeId) const {
	if (!_flow.contains("nodes") || !_flow["nodes"].contains(nodeId)) {
		throw std::runtime_error("Unknown flow node: " + nodeId);
	}
	return _flow["nodes"].at(nodeId);
}

void _FlowManager::StartEiga(const nlohmann::json& node) {
	if (not node.contains("file") || not node["file"].is_string()) {
		throw std::runtime_error("Eiga node requires a string 'file' property.");
	}

	auto eiga = std::make_shared<EigaRuntime>();
	eiga->SetEigaFile(node.at("file").get<std::string>());
	eiga->Load();

	if (node.contains("next")) {
		const std::string nextNodeId = node.at("next").get<std::string>();
		eiga->Finished.Connect([this, nextNodeId]() {
			GoTo(nextNodeId);
			}, true);
	}

	SceneManager.TransSceneTo(eiga);
	eiga->PreloadAssets();
	eiga->Start();
}

void _FlowManager::StartScene(const nlohmann::json& node) {
	if (not node.contains("scene") || not node["scene"].is_string()) {
		throw std::runtime_error("Scene node requires a string 'scene' property.");
	}

	const std::string sceneName = node.at("scene").get<std::string>();
	const auto it = _sceneFactories.find(sceneName);
	if (it == _sceneFactories.end()) {
		throw std::runtime_error("Unregistered scene: " + sceneName);
	}

	SceneManager.TransSceneTo(it->second());
}
