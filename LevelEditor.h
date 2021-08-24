#pragma once

#define LM_CURR_LEVEL_NUMBER 0x58c12c
#define LM_CURR_LEVEL_NUMBER_BEING_SAVED 0x7ef584

#include "LMFunctions.h"

#include <string>
#include <filesystem>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

class LevelEditor
{
public:
	static unsigned int getCurrLevelNumber();
	static unsigned int getLevelNumberBeingSaved();
	static bool exportMwl(
		const fs::path& lmExePath, const fs::path& romPath,
		const fs::path& mwlFilePath, unsigned int levelNumber
	);
	static bool exportMap16(
		const fs::path& lmExePath, const fs::path& romPath,
		const fs::path& map16Path
	);
};
