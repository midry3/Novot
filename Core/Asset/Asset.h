#pragma once
#include <vector>
#include <filesystem>
#include <cstdint>
#include <string>

namespace fs = std::filesystem;

class Asset {
protected:
	fs::path file;
	int _handle = -1;
public:
	Asset() = default;
	Asset(const std::string& file) : file(file) {}
	virtual ~Asset() {
		Release();
	}
	virtual int Load();
	bool IsLoaded() const {
		return _handle != -1;
	}
	std::vector<std::uint8_t> GetData();
	virtual void Release();
	std::string GetPath() const {
		return ("Assets" / file).string();
	}
};