#pragma once

#include <cstdint>
#include <Windows.h>

constexpr uintptr_t LM_CURR_ROM_NAME = 0x5c0030;
constexpr uintptr_t LM_CURR_ROM_PATH = 0x7b5ff8;
constexpr uintptr_t LM_EXE_PATH = 0x592438;
constexpr uintptr_t LM_TOOLBAR_HANDLE = 0xDAFDC8;
constexpr uintptr_t LM_MAIN_EDITOR_WINDOW_HANDLE = 0x8B57F8;

class Paths
{
public:
	static const char* getRomName();
	static const char* getRomDir();
	static const char* getLmExePath();
	static HWND* getToolbarHandle();
	static HWND* getMainEditorWindowHandle();
};
