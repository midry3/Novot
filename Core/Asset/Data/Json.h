#pragma once
#include <Asset/Asset.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Json : public Asset {
private:
	json _json;

public:
	Json(const Path& file) : Asset(file){}

	int Load() override;
	void Release() override;

	json Data() const {
		return _json;
	}
};