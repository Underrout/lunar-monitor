#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

#define LM_CURR_ROM_NAME 0x5c0030
#define LM_CURR_ROM_PATH 0x7b5ff8
#define LM_EXE_PATH 0x592438

class Paths
{
public:
	static const char* getRomName();
	static const char* getRomDir();
	static const char* getLmExePath();
};
