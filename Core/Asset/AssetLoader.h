#pragma once
#include "Asset.h"
#include <functional>
#include <Singleton.h>
#include <string>
#include <map>
#include <filesystem>
#include <memory>
#include <concepts>

namespace fs = std::filesystem;

struct AssetHandle {
	std::shared_ptr<Asset> asset;
	std::vector<int> references;
};

class _AssetLoader : public Singleton<_AssetLoader> {
protected:
	fs::path debugRootDir = "Assets";
	std::map<std::string, AssetHandle> _assetHandles;
	std::map<std::string, std::function<void()>> _assetReleasedCallbacks;
	void CallAssetReleasedCallbacks(const std::string& path) {
		auto it = _assetReleasedCallbacks.find(path);
		if (it != _assetReleasedCallbacks.end()) {
			it->second();
			_assetReleasedCallbacks.erase(it);
		}
	}
public:
	_AssetLoader() = default;
	~_AssetLoader() = default;
	std::map<std::string, AssetHandle> GetAssetHandles() const {
		return _assetHandles;
	}
	void RegisterAssetReleasedCallback(const std::string& path, std::function<void()> callback) {
		_assetReleasedCallbacks[path] = callback;
	}
	template <std::derived_from<Asset> T>
	std::shared_ptr<T> LoadAsset(const std::string& path, int id) {
		if (_assetHandles.find(path) != _assetHandles.end()) {
			_assetHandles[path].references.push_back(id);
			return std::static_pointer_cast<T>(_assetHandles[path].asset);
		}
		std::shared_ptr<T> asset = std::make_shared<T>(path);
		asset->Load();
		_assetHandles[path] = {asset, {id}};
		return asset;
	}
	template <std::derived_from<Asset> T>
	void ReleaseAsset(const std::string& path, int id) {
		auto it = _assetHandles.find(path);
		if (it != _assetHandles.end()) {
			it->second.references.erase(std::remove(it->second.references.begin(), it->second.references.end(), id), it->second.references.end());
			if (it->second.references.empty()) {
				it->second.asset->Release();
				_assetHandles.erase(it);
				CallAssetReleasedCallbacks(path);
			}
		}
	}
	void ReleaseAsset(int id) {
		std::vector<std::string> removeTargets;
		for (auto& [path, handle] : _assetHandles) {
			handle.references.erase(std::remove(handle.references.begin(), handle.references.end(), id), handle.references.end());
			if (handle.references.empty()) {
				handle.asset->Release();
				removeTargets.push_back(path);
				CallAssetReleasedCallbacks(path);
			}
		}
		for (const auto& path : removeTargets) {
			_assetHandles.erase(_assetHandles.find(path));
		}
	}
};

#define AssetLoader _AssetLoader::GetInstance()