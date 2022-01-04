#pragma once
#include "LMFunctions.h"

#include <filesystem>

constexpr uintptr_t LM_CURR_LEVEL_NUMBER = 0x592134;
constexpr uintptr_t LM_CURR_LEVEL_NUMBER_BEING_SAVED = 0x7FAD34;


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
