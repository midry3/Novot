#include "AnimationLoader.h"
#include <fstream>
#include <Asset/AssetManager.h>
#include <stdexcept>
#include <Debugger/Debugger.h>

using namespace AnimationInfoKeys;

AnimationInfo AnimationLoader::LoadAnimationInfo(const fs::path& file) {
	AnimationInfo info;
	std::ifstream ifs(AssetManager.ResolvePath(file.string() + ANIM_EXT));
	if (ifs.is_open()) {
		json data;
		ifs >> data;
		info.rows = data.value(KEY_ROWS, 1);
		info.columns = data.value(KEY_COLUMNS, 1);
		info.loop = data.value(KEY_LOOP, true);
		info.fps = data.value(KEY_FPS, 10);
		info.scale = data.value(KEY_SCALE, 1.0);
	}
	else {
		DebuggerWriteLine("Failed to open animation info file: " + file.string() + ANIM_EXT);
	}
	return info;
}