#pragma once
#include "LMFunctions.h"
#include "BuildResultUpdater.h"

#include <filesystem>

constexpr uintptr_t LM_CURR_LEVEL_NUMBER = 0x59B3B4;
constexpr uintptr_t LM_CURR_LEVEL_NUMBER_BEING_SAVED = 0x8049E0;

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
	static bool exportAllMwls(const fs::path& lmExePath, const fs::path& romPath, 
		const fs::path& mwlFilePath);
	static bool exportMap16(const fs::path& map16Path);
	static void reloadROM(HWND lmRequestWindowHandle, DWORD verificationCode);
};
