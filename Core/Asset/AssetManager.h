#pragma once
#include "Asset.h"
#include <functional>
#include <Singleton.h>
#include <string>
#include <map>
#include <filesystem>
#include <memory>
#include <concepts>
#include <Windows.h>

namespace fs = std::filesystem;

namespace {
	bool IsUtf8(const std::string& data, size_t offset = 0) {
		for (size_t i = offset; i < data.size();) {
			const unsigned char c = static_cast<unsigned char>(data[i]);
			size_t sequenceLength = 0;
			if (c <= 0x7F) {
				sequenceLength = 1;
			}
			else if (c >= 0xC2 && c <= 0xDF) {
				sequenceLength = 2;
			}
			else if (c >= 0xE0 && c <= 0xEF) {
				sequenceLength = 3;
			}
			else if (c >= 0xF0 && c <= 0xF4) {
				sequenceLength = 4;
			}
			else {
				return false;
			}

			if (i + sequenceLength > data.size()) {
				return false;
			}
			for (size_t j = 1; j < sequenceLength; ++j) {
				const unsigned char continuation = static_cast<unsigned char>(data[i + j]);
				if ((continuation & 0xC0) != 0x80) {
					return false;
				}
			}
			i += sequenceLength;
		}
		return true;
	}

	std::string ConvertUtf8ToCp932(const std::string& data, size_t offset) {
		const char* utf8 = data.data() + offset;
		const int utf8Length = static_cast<int>(data.size() - offset);
		const int wideLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, utf8Length, nullptr, 0);
		if (wideLength == 0) {
			return {};
		}

		std::wstring wide(wideLength, L'\0');
		if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, utf8Length, wide.data(), wideLength) == 0) {
			return {};
		}

		const int cp932Length = WideCharToMultiByte(932, WC_NO_BEST_FIT_CHARS, wide.data(), wideLength, nullptr, 0, nullptr, nullptr);
		if (cp932Length == 0) {
			return {};
		}

		std::string result(cp932Length, '\0');
		if (WideCharToMultiByte(932, WC_NO_BEST_FIT_CHARS, wide.data(), wideLength, result.data(), cp932Length, nullptr, nullptr) == 0) {
			return {};
		}
		return result;
	}
}

struct AssetHandle {
	std::shared_ptr<Asset> asset;
	std::vector<int> references;
};

class _AssetManager : public Singleton<_AssetManager> {
protected:
	fs::path _rootDir = "Assets";
	std::map<fs::path, AssetHandle> _assetHandles;
	std::map<fs::path, std::function<void()>> _assetReleasedCallbacks;
	void CallAssetReleasedCallbacks(const fs::path& path) {
		auto it = _assetReleasedCallbacks.find(path);
		if (it != _assetReleasedCallbacks.end()) {
			it->second();
			_assetReleasedCallbacks.erase(it);
		}
	}
public:
	std::map<fs::path, AssetHandle> GetAssetHandles() const {
		return _assetHandles;
	}
	void RegisterAssetReleasedCallback(const fs::path& path, std::function<void()> callback) {
		_assetReleasedCallbacks[path] = callback;
	}

	void Cd(const fs::path& path) {
		_rootDir = path;
	}

	template <std::derived_from<Asset> T>
	std::shared_ptr<T> LoadAsset(const fs::path& path, int id) {
		if (_assetHandles.find(path) != _assetHandles.end()) {
			_assetHandles[path].references.push_back(id);
			return std::static_pointer_cast<T>(_assetHandles[path].asset);
		}
		std::shared_ptr<T> asset = std::make_shared<T>(path);
		asset->Load();
		_assetHandles[path] = { asset, {id} };
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
		std::vector<fs::path> removeTargets;
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

	void ReLoadAssets() {
		for (auto& [_, handle] : _assetHandles) {
			handle.asset->ReLoad();
		}
	}

	fs::path GetPath(const fs::path& path) const {
		return _rootDir / path;
	}

	std::vector<uint8_t> ReadData(const std::string& file) {
		std::vector<uint8_t> data;
		FILE* fp = nullptr;
		fs::path filePath = GetPath(file);
		fopen_s(&fp, filePath.string().c_str(), "rb");
		if (fp == nullptr) {
			return data;
		}
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		data.resize(size);
		fread(data.data(), 1, size, fp);
		fclose(fp);
		return data;
	}

	std::string ReadAsString(const std::string& file) {
		std::string data;
		FILE* fp = nullptr;
		fs::path filePath = GetPath(file);
		fopen_s(&fp, filePath.string().c_str(), "rb");
		if (fp == nullptr) {
			return data;
		}
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		data.resize(size);
		fread(data.data(), 1, size, fp);
		fclose(fp);

		const bool hasUtf8Bom = data.size() >= 3 &&
			static_cast<unsigned char>(data[0]) == 0xEF &&
			static_cast<unsigned char>(data[1]) == 0xBB &&
			static_cast<unsigned char>(data[2]) == 0xBF;
		const size_t contentOffset = hasUtf8Bom ? 3 : 0;
		if (contentOffset != 0) {
			data.erase(0, contentOffset);
		}
		return data;
	}

	fs::path ResolvePath(const fs::path& path) const {
		return GetPath(path);
	}
};

#define AssetManager _AssetManager::GetInstance()