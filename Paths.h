#pragma once

#include <cstdint>

constexpr uintptr_t LM_CURR_ROM_NAME = 0x5C6B98;
constexpr uintptr_t LM_CURR_ROM_PATH = 0x7BD990;
constexpr uintptr_t LM_EXE_PATH = 0x598478;

class Paths
{
public:
	static const char* getRomName();
	static const char* getRomDir();
	static const char* getLmExePath();
};
