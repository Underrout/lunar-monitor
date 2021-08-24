#pragma once

#include <cstdint>

constexpr uintptr_t LM_CURR_ROM_NAME = 0x5c0030;
constexpr uintptr_t LM_CURR_ROM_PATH = 0x7b5ff8;
constexpr uintptr_t LM_EXE_PATH = 0x592438;

class Paths
{
public:
	static const char* getRomName();
	static const char* getRomDir();
	static const char* getLmExePath();
};
