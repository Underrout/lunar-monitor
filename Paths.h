#pragma once

#include <filesystem>
#include <cstdint>
#include <Windows.h>

constexpr uintptr_t LM_CURR_ROM_NAME = 0x5C6B98;
constexpr uintptr_t LM_CURR_ROM_PATH = 0x7BD990;
constexpr uintptr_t LM_EXE_PATH = 0x598478;
constexpr uintptr_t LM_TOOLBAR_HANDLE = 0xDBF5A0;
constexpr uintptr_t LM_MAIN_EDITOR_WINDOW_HANDLE = 0x8C3844;
constexpr uintptr_t LM_MAIN_STATUSBAR_HANDLE = 0xDBF594;

class Paths
{
public:
	static const char* getRomName();
	static const char* getRomDir();
	static const char* getLmExePath();
	static std::filesystem::path getRomPath();
	static HWND* getToolbarHandle();
	static HWND* getMainEditorWindowHandle();
	static HWND* getMainEditorStatusbarHandle();
};
