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
	Asset(const fs::path& file) : file(file) {}
	~Asset() = default;
	virtual int Load() { return -1; }
	virtual void Release() {};
	int GetHandle();
	int ReLoad();
	bool IsLoaded() const {
		return _handle != -1;
	}
	std::vector<std::uint8_t> GetData();
	std::string GetDataAsString();
	fs::path GetPath() const;
	fs::path GetPathRelatedToExe() const;
};