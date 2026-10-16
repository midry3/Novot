#pragma once
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace AnimationInfoKeys {
	const std::string KEY_ROWS = "rows";
	const std::string KEY_COLUMNS = "columns";
	const std::string KEY_LOOP = "loop";
	const std::string KEY_FPS = "fps";
	const std::string KEY_SCALE = "scale";

	const std::string ANIM_EXT = ".anim";
}

struct AnimationInfo {
	int rows=1, columns=1, fps=10;
	double scale = 1.0;
	bool loop=true;
};

class AnimationLoader {
public:
	static AnimationInfo LoadAnimationInfo(const fs::path& file);
};